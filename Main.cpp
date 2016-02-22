#include <algorithm>
#include <cassert>
#include <sstream>

#include "value.hpp"
#include "node.hpp"
#include "parser.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	string filename = argc > 1 ? argv[1] : "input.txt";
	parser a(filename);
	auto prog = a.parse();
	prog->exec(null);
	return 0;
}