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
        
        private:
            void _create_routes();

        public:
            Server();
        };
    }
}
