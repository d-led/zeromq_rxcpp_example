#pragma once

#include <atomic>

class cancellation_token {
    std::atomic<bool> cancelled_;

public:
	cancellation_token() : cancelled_(false) {}

    inline void cancel() {
        cancelled_ = true;
    }

    inline bool cancelled() const {
        return cancelled_;
    }
};
