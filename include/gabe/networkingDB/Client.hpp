#pragma once

#include <string>
#include <memory>

namespace gabe {
    namespace networkingDB {
        class Client
        {
        private:
            std::string _address = "";
            std::string _username = "";
            std::string _password = "";

            uint64_t _network_id = 0;
        public:
            std::shared_ptr<uint8_t> _instances;
        
        public:
            const std::string name;

        public:
            Client();
            Client(const std::string &name);
            ~Client();

            void connect(const std::string &host, const uint16_t& port);
            void connect(const std::string &host, const uint16_t& port, const std::string &username, const std::string &password);

            void disconnect();
        };
    }
}
