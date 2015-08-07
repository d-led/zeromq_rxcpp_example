#define PICOJSON_USE_INT64
#include <picojson_serializer.h> //including before any mention of max macro

#include "../common/zmq_cancellation_token.h"
#include "../common/dtos.h"

#include <iostream>
#include <string>
#include <sole.hpp>

#include <zmq.hpp>
#include <rxcpp/rx.hpp>

int main() {
    zmq::context_t context(1);
    zmq_cancellation_token token(context);
    
    auto uuid = sole::uuid0();
    auto uuid_string = uuid.base62();

    std::cout<<"worker: "<< uuid_string <<std::endl;

    auto scheduler = rxcpp::schedulers::make_same_worker(rxcpp::schedulers::make_event_loop().create_worker());
    auto coordination = rxcpp::identity_one_worker(scheduler);

    auto heartbeat = rxcpp::observable<>::interval(
                // when to start
                scheduler.now(),
                std::chrono::seconds(1),
                coordination)
            .map([&](int c) -> worker_heartbeat { return { uuid_string, c }; });

    zmq::socket_t heartbeat_socket (context, ZMQ_PUSH);

    heartbeat_socket.connect ("tcp://localhost:5555");

    heartbeat.subscribe([&uuid,&heartbeat_socket](worker_heartbeat hb) {
        std::string val(picojson::convert::to_string(hb));
        zmq::message_t msg(val.length());
        memcpy((void *)msg.data(), val.data(), val.size());
        heartbeat_socket.send(msg);
    });

    token.wait();
}
