#pragma once

#include "locked-map.hpp"
#include "parts/part-io.h"

namespace donkeycar {

// Memory provides thread-safe reads and writes of key-value pairs
class Memory {
    typedef std::shared_ptr<donkeycar::PartIO> Datum;

   public:
    explicit Memory(){};
    Memory(const Memory&) = delete;
    Memory& operator=(const Memory&) = delete;
    ~Memory() = default;

    // TODO: support list of keys
    Datum get(const std::string& key = "") {
        Datum msg;
        try {
            // msg = std::move(m_data.at(key));
            msg = m_data.at(key);
        } catch (std::out_of_range e) {
            return nullptr;
        }
        return msg;
    }
    void put(const std::string& key, const Datum& msg) {
        // move semantics on insert
        m_data.insert(key, msg);
    }

   protected:
    concurrency::LockedMap<std::string, Datum> m_data;
};
}  // namespace donkeycar