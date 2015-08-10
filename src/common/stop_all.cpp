#include "zmq_cancellation_token.h" //for the travis compat hack
#include <zmq.hpp>
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    std::cout << "stop_all: stopping everybody" << std::endl;

    zmq::context_t context(1);
    zmq::socket_t kill_pub(context, ZMQ_PUB);
    kill_pub.bind("tcp://*:5556");

    for (auto i = 0; i < 3; i++) {
        static const char* stop = "stop";
        zmq::message_t msg(strlen(stop) + 1);
        snprintf((char*)msg.data(), strlen(stop) + 1, "%s", stop);

        kill_pub.send(msg);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
