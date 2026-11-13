#pragma once

#include <string>
#include <filesystem>
#include <system_error>
#include <algorithm>
#include <cctype>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace Core {
namespace PathUtils {

namespace fs = std::filesystem;

inline std::string pathToUtf8(const fs::path& p) {
#if defined(_WIN32)
    std::wstring ws = p.wstring();
    if (ws.empty()) return std::string();
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, ws.data(), static_cast<int>(ws.size()), nullptr, 0, nullptr, nullptr);
    if (sizeNeeded <= 0) return std::string();
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, ws.data(), static_cast<int>(ws.size()), &result[0], sizeNeeded, nullptr, nullptr);
    return result;
#else
    return p.string();
#endif
}

inline fs::path utf8ToPath(const std::string& str) {
#if defined(_WIN32)
    if (str.empty()) return fs::path();
    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0);
    if (sizeNeeded <= 0) return fs::path();
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), &result[0], sizeNeeded);
    return fs::path(result);
#else
    return fs::path(str);
#endif
}

inline fs::path weaklyCanonicalSafe(const fs::path& p) {
    std::error_code ec;
    fs::path result = fs::weakly_canonical(p, ec);
    if (ec || result.empty()) {
        return p.lexically_normal();
    }
    return result.lexically_normal();
}

inline std::string toLowerString(const std::string& str) {
    std::string s = str;
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

inline std::string pathNormalizedKey(const fs::path& p) {
    fs::path norm = weaklyCanonicalSafe(p);
#if defined(_WIN32)
    return toLowerString(pathToUtf8(norm));
#else
    return pathToUtf8(norm);
#endif
}

} // namespace PathUtils
} // namespace Core
