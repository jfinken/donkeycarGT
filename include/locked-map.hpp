#pragma once

#include <optional>
#include <unordered_map>
#include "lock.hpp"
#include "mutex.h"

namespace donkeycar::concurrency {

/**
 * @brief Class providing an unordered-map with operations protected via a
 * custom lock (RAII). Likely Intel's TBB (tbb::concurrent_unordered_map) or
 * Boost's boost::unordered_map (with appropriate locking) are going to be of
 * higher performance.  Alternatively they may not be available on the given
 * target/platform.
 * @tparam KeyType
 * @tparam DataType
 */
template <class KeyType, class DataType>
class LockedMap {
   public:
    /**
     * @brief Construct a new LockedMap object but disable copy-construction and
     * assignment
     */
    explicit LockedMap();
    LockedMap(const LockedMap&) = delete;
    LockedMap& operator=(const LockedMap&) = delete;
    ~LockedMap();

    /**
     * @brief Clear the map
     */
    void clear();

    /**
     * @brief Insert into the map
     *
     * @param key
     * @param value
     */
    void insert(const KeyType& key, const DataType& value);

    /**
     * @brief Retrieve from the map with bounds-checking.  It will throw
     * std::out_of_range if the container does not have a value at key
     */
    // const DataType& at(const KeyType& key);
    DataType at(const KeyType& key);

    // TODO: consider the operator[] overload

    /**
     * @brief Get the size of the map
     *
     * @return size_t
     */
    size_t size() const;

    /**
     * @brief Checks if the map is empty
     *
     * @return true
     * @return false
     */
    bool empty() const;

   private:
    mutable donkeycar::concurrency::Mutex m_mutex{"LockedMap"};
    std::unordered_map<KeyType, DataType> m_data;
};

// Impl
template <class KeyType, class DataType>
LockedMap<KeyType, DataType>::LockedMap() {}

template <class KeyType, class DataType>
LockedMap<KeyType, DataType>::~LockedMap() {
    clear();
}
template <class KeyType, class DataType>
void LockedMap<KeyType, DataType>::insert(const KeyType& key,
                                          const DataType& value) {
    donkeycar::concurrency::Lock lock(m_mutex);
    m_data[key] = value;
}
template <class KeyType, class DataType>
DataType LockedMap<KeyType, DataType>::at(const KeyType& key) {
    // RAII, will lock the mutex on construction and unlock during
    // destruction (when this local variable goes out of scope)
    donkeycar::concurrency::Lock lock(m_mutex);
    return m_data.at(key);
}
// clear the map, including possibly deleting objects on the heap
template <class KeyType, class DataType>
void LockedMap<KeyType, DataType>::clear() {
    donkeycar::concurrency::Lock lock(m_mutex);
    // could be pointers to objects on the heap
    if constexpr (std::is_pointer<DataType>::value) {
        using Map = std::unordered_map<KeyType, DataType>;
        typename Map::iterator mit = m_data.begin();
        typename Map::iterator emit = m_data.end();

        for (; mit != emit; ++mit) {
            delete mit->second;
        }
    }
    m_data.clear();
}
template <class KeyType, class DataType>
size_t LockedMap<KeyType, DataType>::size() const {
    donkeycar::concurrency::Lock lock(m_mutex);
    return m_data.size();
}
template <class KeyType, class DataType>
bool LockedMap<KeyType, DataType>::empty() const {
    donkeycar::concurrency::Lock lock(m_mutex);
    return m_data.empty();
}

}  // namespace donkeycar::concurrency