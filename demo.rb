require 'rufus-scheduler'

scheduler = Rufus::Scheduler.new

def start(what)
	pipe = IO.popen(what)
	pipe.each do |line|
	  puts line.chomp
	end
	pipe.close
end

def start_demo(what)
	os = 'linux'
	os = 'macosx' if `uname`.downcase.start_with? 'darwin'

	start 'bin/' + os + '/gmake/x64/Release/' + what
end

scheduler.in '1s' do
	start_demo 'worker'
end

scheduler.in '2s' do
	start_demo 'server'
end

scheduler.in '3s' do
	start_demo 'worker'
end

scheduler.in '4s' do
	start_demo 'worker'
end

scheduler.in '15s' do
  start_demo 'stop_all'
end

scheduler.in '18' do
  puts 'bye'
  exit
end

scheduler.join
