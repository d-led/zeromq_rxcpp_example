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
	bitness = 
	start_dir = "bin/linux/gmake/x#{1.size * 8}/Release/"
	start_dir = "bin/macosx/gmake/x#{1.size * 8}/Release/" if `uname`.downcase.start_with? 'darwin'

	start start_dir + what
end

[
	['1s','worker'],
	['2s','server'],
	['3s','worker'],
	['4s','worker'],
	['15s','stop_all'],
].each do |task|
	scheduler.in task[0] do
		puts "starting #{task[1]}"
		start_demo task[1]
	end
end

scheduler.in '18s' do
  puts 'bye'
  exit
end

scheduler.join
