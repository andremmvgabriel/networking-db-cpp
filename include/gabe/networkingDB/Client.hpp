#pragma once

#include <string>
#include <memory>
#include <unordered_map>

namespace gabe {
    namespace networkingDB {
        class Client
        {
        private:
            std::string _address = "";
            std::string _username = "";
            std::string _password = "";

            uint64_t _network_id = 0;

            std::unordered_map<std::string, uint64_t> _topics_map;
            // std::unordered_map<std::string, uint64_t> _callbacks_map;

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

            void subscribe(const std::string &topic);
            // void subscribe(const std::string &topic, CALLBACK);
            void unsubscribe(const std::string &topic);
            void unsubscribe_all();

            void send(const std::string &topic, const std::string &message);
            std::string receive(const std::string &topic);
        };
    }
}
