#define PICOJSON_USE_INT64
#include <picojson_serializer.h> //including before any mention of max macro

#include <iostream>
#include <zmq.hpp>
#include <rxcpp/rx.hpp>

#include "../common/zmq_cancellation_token.h"
#include "../common/dtos.h"

int main() {

    zmq::context_t context(1);
    zmq_cancellation_token token(context);

    // socket config
    std::string port="5555";
    std::string socket_config="tcp://*:";
    socket_config+=port;

    zmq::socket_t pull_from_workers(context, ZMQ_PULL);
    pull_from_workers.bind(socket_config.c_str());

    std::cout<<"Starting listening at "<<socket_config<<std::endl;
    
    // publish stream of heartbeats
    auto worker_heartbeats = rxcpp::observable<>::
        create<worker_heartbeat>(
            [&pull_from_workers,&token](rxcpp::subscriber<worker_heartbeat> out){
                while (true) {
                    zmq::message_t request;
                    pull_from_workers.recv(&request);
                    worker_heartbeat res;
                    picojson::convert::from_string(std::string(static_cast<char*>(request.data()), request.size()), res);
                    out.on_next(res);
                }
                out.on_completed();
            }).
        as_dynamic().
        subscribe_on(rxcpp::synchronize_new_thread()).
        publish();

    // non-blocking subscription
    auto grouped_heartbeats = worker_heartbeats
        .group_by(
            [](worker_heartbeat const& s) { return s.id; },
            [](worker_heartbeat const& s) { return s.beat; }
            )
        ;

    grouped_heartbeats.subscribe(
        [](rxcpp::grouped_observable<std::string, int64_t> g) {
            auto key = g.get_key();
            g.subscribe(
                [key](int64_t beat) {
                    std::cout << key << ":" << beat << std::endl;
                }
            );
        }
    );

    // todo: worker appearing and disappearing identifiable via timeout
    // todo: remove explicit heartbeat output

    // start loop on input thread and block until complete
    worker_heartbeats.
        connect_forever().
        subscribe();

    token.wait();
}
