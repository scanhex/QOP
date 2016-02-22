//
// Created by Alexander Morozov on 20/02/16.
//

#include "parser.hpp"

yyFlexLexer lexer(new ifstream("input.txt"));

bool parser::func_exists(string name)
{
    return built_in_contains(name) || func_parsed.count(name);
}

void parser::fail(string s)
{
    std::cerr << "Syntax parse error on line " << yyline << ". Error message: " << s << std::endl;
}


parser::parser()
{
    move_next();
}

parser::parser(string name)
{
    lexer = yyFlexLexer(new ifstream(name));
    move_next();
}

nd *parser::parse()
{
    return parse(0);
}

op_type parser::get_op_type(string op)
{
    if (op == "&")
        return AND;
    if (op == "|")
        return OR;
    if (op == "^")
        return XOR;
    if (op == "=")
        return ASSIGN;
    if (op == "wh")
        return WHILE;
    if (op == "if")
        return IF;
    if (op == "&&")
        return GAND;
    if (op == "||")
        return GOR;
    if (op == "<")
        return LESS;
    if (op == ">")
        return GREATER;
    if (op == "?")
        return EQUALS;
    if (op == "+")
        return PLUS;
    if (op == "-")
        return MINUS;
    if (op == "*")
        return MULT;
    if (op == "/")
        return DIV;
    if (op == "%")
        return MOD;
    return XOR;
}

op_type token;

void parser::move_next()
{
    if (token == NEWLINE)
        while (token == NEWLINE)
            token = (op_type) lexer.yylex();
    else
        token = (op_type) lexer.yylex();
    while (token == SPACE)
        token = (op_type) lexer.yylex();
    if (token == ID)
    {
        if (func_exists(str))
            token = FUNC_CALL;
        else
            token = VAR;
    }
}

nd *parser::parse_leaf()
{
    if (token == NOT)
    {
        move_next();
        return new nd(NOT, parse_leaf(), null);
    }
    if (token == MINUS)
    {
        move_next();
        return new nd(NEG, parse_leaf(), null);
    }
    if (token == LPAREN)
    {
        move_next();
        nd *ans = parse();
        assert(token == RPAREN);
        move_next();
        return ans;
    }
    if (token == LBRACKET)
    {
        move_next();
        nd *ans = parse();
        if (token != RBRACKET)
            fail("Bad brackets in expression");
        assert(token == RBRACKET);
        move_next();
        return new nd(BRACKETS, ans, null);
    }
    if (token == LBRACE)
    {
        move_next();
        nd *ans = parse();
        assert(token == RBRACE);
        move_next();
        return new nd(BRACES, ans, null);
    }
    if (token == FUNC_CALL)
    {
        string name = str;
        move_next();
        vector<nd *> args = parse_args();
        args.insert(args.begin(), new nd(VAL, VALUE((int) args.size())));
        for (int i = 0; i < 10; ++i)
            args.push_back(new nd(VAL, VALUE()));
        VALUE *tup = new VALUE[args.size()];
        for (int i = 0; i < args.size(); ++i)
            tup[i] = VALUE(VALUE::NODE, args[i]);
        return new nd(FUNC_CALL, new nd(VAL, VALUE(VALUE::STRING, new string(name))),
                      new nd(VAL, VALUE(VALUE::TUPLE, tup)));
    }
    if (token == WHILE)
    {
        move_next();
        nd *expr = parse(get_prior(NEWLINE) + 1);
        move_next(); // skip newline
        assert(token == LPAREN);
        move_next();
        nd *body = parse();
        assert(token == RPAREN);
        move_next();
        return new nd(WHILE, expr, body);
    }
    if (token == IF)
    {
        move_next();
        nd *expr = parse(get_prior(NEWLINE) + 1);
        move_next(); // skip newline
        assert(token == LPAREN);
        move_next();
        nd *body = parse();
        if (token != RPAREN)
            fail("Bad IF parentheses");
        assert(token == RPAREN);
        move_next();
        return new nd(IF, expr, body);
    }
    if (token == FUNC_DEF)
    {
        move_next();
        assert(token == VAR || token == FUNC_CALL);
        string name = str;
        func_parsed.insert(name);
        move_next(); // skip name
        move_next(); // skip newline
        assert(token == LPAREN);
        move_next();
        nd *body = parse();
        if (token != RPAREN)
            fail("Bad parentheses in function definition");
        assert(token == RPAREN);
        move_next(); // skip )
        return new nd(FUNC_DEF, new nd(VAL, VALUE(VALUE::STRING, new string(name))), body);
    }
    if (token == VAR)
    {
        string s = str;
        move_next();
        return new nd(VAR, VALUE(VALUE::STRING, new string(s)));
    }
    if (token == RETURN)
    {
        move_next();
        return new nd(RETURN, parse(), null);
    }
    if (token == NUMBER)
    {
        int x = num;
        move_next();
        return new nd(NUMBER, VALUE(x));
    }
    if (token == NIL)
        return new nd(NIL, VALUE());
    return new nd(NIL, VALUE());
}

int parser::get_prior(op_type op)
{
    if (op == NEWLINE)
        return 0;
    if (op == AND)
        return 1;
    if (op == OR)
        return 1;
    if (op == XOR)
        return 1;
    if (op == GAND)
        return 2;
    if (op == GOR)
        return 2;
    if (op == ASSIGN)
        return 3;
    if (op == LESS)
        return 4;
    if (op == GREATER)
        return 4;
    if (op == EQUALS)
        return 4;
    if (op == PLUS)
        return 5;
    if (op == MINUS)
        return 5;
    if (op == MULT)
        return 6;
    if (op == DIV)
        return 6;
    if (op == MOD)
        return 6;
    return -1;
}

nd *parser::parse(int prior)
{
    if (prior == 7)
        return parse_leaf();
    nd *x = parse(prior + 1);
    while (get_prior(token) == prior)
    {
        auto tok = token;
        move_next();
        auto nw = parse(prior + 1);
        if (nw->left == null && nw->right == null && nw->value.t == VALUE::VOID)
            continue;
        x = new nd(tok, x, nw);
    }
    return x;
}

vector<nd *> parser::parse_args()
{
    if (token != LPAREN)
        fail("Bad parentheses in function call");
    assert(token == LPAREN);
    vector<nd *> args;
    move_next();
    if (token == RPAREN)
    {
        move_next();
        return args;
    }
    args.push_back(parse());
    while (token == COMMA)
        move_next(), args.push_back(parse());
    assert(token == RPAREN);
    move_next();
    return args;
}
