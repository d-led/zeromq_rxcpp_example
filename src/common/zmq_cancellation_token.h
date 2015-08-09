#pragma once

#include "cancellation_token.h"

// travis hack
#ifndef ZMQ_STREAM
#define ZMQ_STREAM 0
#endif

#include <zmq.hpp>


class zmq_cancellation_token {
    zmq::context_t& ctx;
    cancellation_token token;
    zmq::socket_t sub;

    inline void connect() {
        sub.connect("tcp://localhost:5556");
        static const char* stop = "st";
        sub.setsockopt(ZMQ_SUBSCRIBE, stop, strlen(stop));
    }

public:
    // blocks until cancelled
    inline zmq_cancellation_token(zmq::context_t& c) :
        ctx(c),
        sub(c,ZMQ_SUB)
    {
        connect();
    }

public:
    inline void wait() {
        while (!token.cancelled()) {
            zmq::message_t stop;
            sub.recv(&stop);
            token.cancel();
        }
    }

    inline bool cancelled() const {
        return token.cancelled();
    }
};
