#pragma once

#include <tuple>
#include <string>
#include <chrono>
#include <format>
#include <thread>
#include <cstdint>
#include <iostream>
#include <filesystem>

namespace Yum { namespace Yum_cxxlibs { namespace fs {


inline int64_t file_size(const std::string &path) {
  return static_cast<int64_t>(std::filesystem::file_size(path));
}

inline bool file_exists(const std::string &path) {
  return std::filesystem::exists(path);
}

inline bool is_directory(const std::string &path) {
  return std::filesystem::is_directory(path);
}

inline bool create_directory(const std::string &path) {
  return std::filesystem::create_directories(path);
}







} } }