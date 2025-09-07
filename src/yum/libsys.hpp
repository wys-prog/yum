#pragma once

#include <tuple>
#include <string>
#include <chrono>
#include <format>
#include <thread>
#include <cstdint>
#include <iostream>
#include <filesystem>

namespace Yum { namespace Yum_cxxlibs { namespace sys {

  int64_t execute(const std::string &cmd) {
    return std::system(cmd.c_str());
  }

  void sleep(int64_t duration) {
    std::this_thread::sleep_for(std::chrono::microseconds(duration));
  }

  void Yexit(int64_t code) {
    std::exit(code);
  }

  /* * Future functions
   ** sys.osname(void) -> string
   ** sys.tmpdir(void) -> string
   ** sys.env(key: string) -> string ('' if null)
   ** sys.is_env(key: string) -> bool
   ** MAP: sys.date
           sys.date.time(format: string) -> string
           sys.date.unix_time() -> int
      -- UNSURE --
      MAP: sys.dll
           sys.dll.open(name) -> ???
           sys.dll.get(symbolname: string) -> ???
  */

} } }