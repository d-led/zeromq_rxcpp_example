#include <iostream>
#include <zmq.hpp>
#include <rxcpp/rx.hpp>
#include <sole.hpp>

int main() {
	auto uuid = sole::uuid0();
	std::cout<<"worker: "<<uuid<<std::endl;
}
