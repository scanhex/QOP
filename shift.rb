f = File.new('shift.out', 'w')
while s = gets
	s.chomp!
	s1 = s.scan(/./).each do |x| 
		f.print (if '0' <= x && x <= '9'
			(x.to_i + 1).to_s
		else
			x
		end)
	end
	f.puts
end