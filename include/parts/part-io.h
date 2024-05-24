#pragma once

namespace donkeycar {
/**
 * @brief Class representing abstract data for Parts. All data for Parts must
 * derive from this class.  This allows objects to be read and written from and
 * to the Vehicle memory store, and thus be delivered and received to and from
 * Parts.
 */
class PartIO {
   public:
    PartIO() = default;

   protected:
    // common data here
    // TODO: timestamp via a common time api, e.g. wrapper around the chrono
    // monotonic clock
};
}  // namespace donkeycar