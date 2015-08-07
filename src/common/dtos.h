#pragma once

#include <inttypes.h>
#include <string>

struct worker_heartbeat {
    std::string id;
    int64_t beat;

    template <class Archive>
    void json(Archive &ar) {
        ar & picojson::convert::member("id", id);
        ar & picojson::convert::member("beat", beat);
    }
};
