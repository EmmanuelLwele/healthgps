#include "program_dirs.h"

#ifdef __linux__
#include <climits>
#include <unistd.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <mach-o/dyld.h>
#endif

#include "HealthGPS.Core/exception.h"

#include <sago/platform_folders.h>

#include <array>

namespace {
constexpr const char *program_name = "healthgps";

void throw_path_error() { throw hgps::core::HgpsException("Could not get program path"); }
} // anonymous namespace

namespace hgps {
std::filesystem::path get_program_directory() { return get_program_path().parent_path(); }

std::filesystem::path get_program_path() {
#if defined(__linux__)
    std::array<char, PATH_MAX> path{};
    if (readlink("/proc/self/exe", path.data(), path.size() - 1) == -1) {
        throw_path_error();
    }
#elif defined(_WIN32)
    std::array<wchar_t, MAX_PATH> path{};
    if (GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size())) == 0) {
        throw_path_error();
    }
#elif defined(__APPLE__)
    std::array<char, PATH_MAX> raw_path{};
    auto size = static_cast<std::uint32_t>(raw_path.size());
    if (_NSGetExecutablePath(raw_path.data(), &size) != 0) {
        throw_path_error();
    }

    // Resolve symlinks and relative components, as /proc/self/exe does on Linux
    std::array<char, PATH_MAX> path{};
    if (realpath(raw_path.data(), path.data()) == nullptr) {
        throw_path_error();
    }
#else
#error "Unsupported platform"
#endif

    return path.data();
}

std::filesystem::path get_cache_directory() {
    return std::filesystem::path{sago::getCacheDir()} / program_name;
}

std::filesystem::path get_temporary_directory() {
    return std::filesystem::temp_directory_path() / program_name;
}
} // namespace hgps
