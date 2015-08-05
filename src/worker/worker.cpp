#include <iostream>
#include <zmq.hpp>
#include <rxcpp/rx.hpp>
#include <uuid.hpp>

int main() {
	auto uuid = Utility::UUID::random().to_string();
	std::cout<<"worker: "<<uuid<<std::endl;
}
