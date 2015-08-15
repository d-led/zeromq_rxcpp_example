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
	absolute_path_to_executable = 	(
										(Dir.glob("bin/**/"+ what +".*")) + 
										(Dir.glob("bin/**/"+ what      ))
									).max_by {|f| File.mtime(f)}

	# puts absolute_path_to_executable
	start absolute_path_to_executable
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
