#include <iostream>
#include <string>
#include <zmq.hpp>
#include <rxcpp/rx.hpp>
#include <sole.hpp>

#include "../common/zmq_cancellation_token.h"

int main() {
    zmq::context_t context(1);
    zmq_cancellation_token token(context);
    auto uuid = sole::uuid0();
    std::cout<<"worker: "<<uuid<<std::endl;

    auto scheduler = rxcpp::schedulers::make_same_worker(rxcpp::schedulers::make_event_loop().create_worker());
    auto coordination = rxcpp::identity_one_worker(scheduler);

    auto heartbeat = rxcpp::observable<>::interval(
                // when to start
                scheduler.now(),
                std::chrono::seconds(1),
                coordination)
            .map([](int c) { return std::to_string(c); });

    zmq::socket_t heartbeat_socket (context, ZMQ_PUSH);

    heartbeat_socket.connect ("tcp://localhost:5555");

    heartbeat.subscribe([&uuid,&heartbeat_socket](std::string const& c) {
        std::string val = uuid.str() + ":" +c;
        zmq::message_t msg(val.length());
        memcpy((void *)msg.data(), val.data(), val.size());
        heartbeat_socket.send(msg);
    });

    token.wait();
}
