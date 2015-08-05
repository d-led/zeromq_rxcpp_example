include 'premake'

make_solution 'zeromq_rxcpp_example'

-- additional
platforms { "x64" }

zeromq_root = {
	x32 = [[C:\Program Files (x86)\ZeroMQ 3.2.4\]],
	x64 = [[C:\Program Files\ZeroMQ 3.2.4\]],
	osx = [[/usr/local/Cellar/zeromq/4.1.2/]]
}

zeromq_lib = {
	win = 'libzmq-v110-mt-3_2_4',
	osx = 'zmq'
}

includedirs {
	'deps/cppzmq',
}

-- libzmq config
configuration {'x32','windows'}
	includedirs { path.join(zeromq_root.x32,'include') }
	libdirs { path.join(zeromq_root.x32,'lib') }
configuration {'x64','windows'}
	includedirs { path.join(zeromq_root.x64,'include') }
	libdirs { path.join(zeromq_root.x64,'lib') }
configuration 'macosx'
	includedirs { path.join(zeromq_root.osx,'include') }
	libdirs { path.join(zeromq_root.osx,'lib') }	
configuration '*'

function deploy_libzmq()
	configuration {'x32','windows'}
		postbuildcommands {
			[[xcopy "]]..path.getabsolute(path.join(zeromq_root.x32,"bin/*.dll")):gsub('/','\\')..[[" "$(TargetDir)" /s /d /y]]
		}
	configuration {'x64','windows'}
		postbuildcommands {
			[[xcopy "]]..path.getabsolute(path.join(zeromq_root.x64,"bin/*.dll")):gsub('/','\\')..[[" "$(TargetDir)" /s /d /y]]
		}
	configuration '*'
end

function link_zeromq()
	configuration 'windows'
		links {
			-- 'dbghelp',
			zeromq_lib.win
		}
	configuration 'macosx'
		links {
			zeromq_lib.osx,
		}
	configuration '*'
end

------------------------------------
make_console_app('server',{
	'src/server/*.h',
	'src/server/*.cpp'
})

	make_cpp11()
	link_zeromq()
	deploy_libzmq()
	
------------------------------------
make_console_app('worker',{
	'src/worker/*.h',
	'src/worker/*.cpp'
})

	make_cpp11()
	link_zeromq()

