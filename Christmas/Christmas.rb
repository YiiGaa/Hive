=begin
 Christmas v0.1 for Once framework
 Designed by Daniel.Leung
=end
# encoding=UTF-8
require_relative 'Kernel/kernel.rb'


puts 'Hi, I am Christmas.I am a tool which would work for Once framework.'
if(ARGV.length != 0) then
	strList = ''
	tMoudleList = []
	ARGV.each do |list|
		strList = strList + ", #{list}"
		tMoudleList << list
	end
	puts "I will make #{strList}"
	ObjKernel = CKernel.new
	ObjKernel.start(tMoudleList)
else
	puts 'Please input one or more modules'
	puts 'like: ruby Lily.rb Rec Play'
	puts 'Rec and Play are modules'
end
