#pragma once

#include <crow.h>

#include <map>

#include <gabe/networkingDB/NetworkDatabase.hpp>

namespace gabe {
    namespace networkingDB {
        class Server
        {
        private:
            crow::SimpleApp _app;
            NetworkDatabase _db;
            
            uint64_t _active_session;
        
        private:
            void _create_sessions_routes();
            void _create_clients_routes();
            void _create_topics_routes();
            void _create_messages_routes();

            std::string _serialize_as_json(const table_data_t& data) const;
            std::string _serialize_sessions_as_readable(const table_data_t& sessions) const;
            std::string _serialize_clients_as_readable(const table_data_t& clients) const;
            std::string _serialize_topics_as_readable(const table_data_t& topics) const;
            std::string _serialize_messages_as_readable(const table_data_t& messages) const;

        public:
            Server();
            Server(const std::string &file_name);

            void start(const uint16_t &port);
        };
    }
}
