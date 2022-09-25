#pragma once

#include <sqlite3.h>
#include <fmt/format.h>

#include <thread>
#include <chrono>
#include <string>
#include <sstream>

namespace gabe {
    namespace networkingDB {
        class Client
        {
        public:
            Client() {}
            Client(const std::string &name) {}

            void connect(const std::string &host, const uint16_t& port) {}
        };
    }
}
