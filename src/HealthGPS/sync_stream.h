#pragma once

#include <version>

#if defined(__cpp_lib_syncstream)
#include <syncstream>
#else
#include <mutex>
#include <ostream>
#include <sstream>
#endif

namespace hgps {

#if defined(__cpp_lib_syncstream)

/// @brief Synchronised output stream (std::osyncstream when available)
using osyncstream = std::osyncstream;

#else

/// @brief Fallback for standard libraries without std::osyncstream (e.g. Apple libc++).
///
/// Buffers output and writes it to the wrapped stream in one locked operation on destruction,
/// matching the usage pattern `hgps::osyncstream(std::cout) << ...;`.
class osyncstream : public std::ostringstream {
  public:
    /// @brief Initialises a new instance of the osyncstream class
    /// @param target The stream to write the buffered output to
    explicit osyncstream(std::ostream &target) : target_{target} {}

    osyncstream(const osyncstream &) = delete;
    osyncstream &operator=(const osyncstream &) = delete;
    osyncstream(osyncstream &&) = delete;
    osyncstream &operator=(osyncstream &&) = delete;

    ~osyncstream() override {
        const auto lock = std::scoped_lock{mutex()};
        target_ << str();
        target_.flush();
    }

  private:
    std::ostream &target_;

    static std::mutex &mutex() {
        static std::mutex instance;
        return instance;
    }
};

#endif

} // namespace hgps
