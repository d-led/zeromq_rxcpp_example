#include <iostream>
#include <zmq.hpp>
#include <rxcpp/rx.hpp>

int main() {

	zmq::context_t context(1);

	// socket config
	std::string port="5555";
	std::string socket_config="tcp://*:";
	socket_config+=port;

	zmq::socket_t pull_from_workers(context, ZMQ_PULL);
	pull_from_workers.bind(socket_config.c_str());

	std::cout<<"Starting listening at "<<socket_config<<std::endl;

	bool to_kill=false;
	
	// publish stream of heartbeats
	auto worker_heartbeats = rxcpp::observable<>::
	    create<std::string>(
	        [&pull_from_workers,&to_kill](rxcpp::subscriber<std::string> out){
	            while (!to_kill) {
					zmq::message_t request;
					pull_from_workers.recv(&request);
					out.on_next(std::string(static_cast<char*>(request.data()),request.size()));
				}
	            out.on_completed();
	        }).
	    as_dynamic().
	    subscribe_on(rxcpp::synchronize_new_thread()).
	    publish();

	// non-blocking subscription
	worker_heartbeats.subscribe([](std::string const& s) {
	    std::cout << s << std::endl;
	});

	// todo: worker appearing and disappearing
	// todo: remove explicit heartbeat output
	// todo: get rid of the blocking observable, as it uses a conditional variable, which boils down to a tight loop

	// start loop on input thread and block until complete
	worker_heartbeats.
	    connect_forever().
	    as_blocking().
	    subscribe();
}
