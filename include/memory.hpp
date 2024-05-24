#pragma once

#include "locked-map.hpp"
#include "parts/part-io.h"

namespace donkeycar {

/**
 * @brief Class Memory provides thread-safe reads and writes of key-value pairs
 */
class Memory {
    typedef std::shared_ptr<donkeycar::PartIO> Datum;

   public:
    /**
     * @brief Construct a new Memory object. Copy-construction and
     * copy-assignment are disabled.
     *
     */
    explicit Memory(){};
    Memory(const Memory&) = delete;
    Memory& operator=(const Memory&) = delete;
    ~Memory() = default;

    /**
     * @brief Return Datum at the given string key. nullptr is returned
     * if the object at key does not exist.
     * TODO: support list of keys?
     *
     * @param key
     * @return Datum
     */
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
    /**
     * @brief Store the object at key in the map.
     *
     * @param key
     * @param msg of type Datum
     */
    void put(const std::string& key, const Datum& msg) {
        m_data.insert(key, msg);
    }

   protected:
    concurrency::LockedMap<std::string, Datum> m_data;
};
}  // namespace donkeycar