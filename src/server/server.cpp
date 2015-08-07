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

            using timeout_t = std::pair<std::chrono::steady_clock::duration, std::chrono::steady_clock::time_point>;

            //auto timeout = g
            //    //.combine_latest(3 second beat)
            //    .map([](int64_t) { return std::chrono::steady_clock::now(); })
            //    .scan(  std::make_pair(std::chrono::steady_clock::duration(),std::chrono::steady_clock::now()),
            //            [](timeout_t last, std::chrono::steady_clock::time_point now) {
            //        return std::make_pair(now - last.second, now);
            //    })
            //    .filter([](timeout_t entry) {
            //        return entry.first > std::chrono::seconds(3);
            //    })
            //;

            //timeout.subscribe([key](timeout_t) {
            //    std::cout << "Worker " << key << " timeout";
            //});
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
