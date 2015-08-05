#include <iostream>

#include <rxcpp/rx.hpp>

int main() {
	// publish input from cin from a single input thread
	// cin does not support multithreaded access
	auto console_lines = rxcpp::observable<>::
	    create<std::string>(
	        [](rxcpp::subscriber<std::string> out){
	            std::string line;
	            while (std::getline(std::cin, line) && !line.empty()) {
	                out.on_next(line);
	            }
	            out.on_completed();
	        }).
	    as_dynamic().
	    subscribe_on(rxcpp::synchronize_new_thread()).
	    publish();

	// non-blocking subscription 1
	console_lines.subscribe([](std::string const& s) {
	    // on input thread
	    std::cout << "1:" << s << std::endl;
	});

	// non-blocking subscription 2
	console_lines.subscribe([](std::string const& s) {
	    // on input thread
	    std::cout << "2:" << s << std::endl;
	});

	// start loop on input thread and block until complete
	console_lines.
	    connect_forever().
	    as_blocking().
	    subscribe();
}
