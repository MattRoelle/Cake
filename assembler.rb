infile = File.open(ARGV[0], 'r')
outfile = File.new(ARGV[1], 'w')

opcodes = {
  :LDA => 0x00,
  :LDX => 0x01,
  :LDY => 0x02,
  :STA => 0x03,
  :STX => 0x04,
  :STY => 0x05,
  :ADD => 0x06,
  :SUB => 0x07,
  :TAX => 0x08,
  :TAY => 0x09,
  :TXA => 0x0A,
  :TYA => 0x0B,
  :TXY => 0x0C
}

IO.readlines(infile).each do |line|
  puts line
  line = line.strip
  instruction = line.split(" ")[0].to_sym
  params = line.split(" ")[1..-1]
  outfile << [opcodes[instruction]].pack('C*')
  params.each do |param|
   if param[0] == '#'
    outfile << [0x00].pack('C*') 
   elsif param[0] == '$'
     outfile << [0x01].pack('C*')
   end
   outfile << param[1..-1].scan(/.{2}/).map(&:hex).pack('C*')
  end
end

infile.close
outfile.close
