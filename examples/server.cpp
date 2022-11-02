#include <gabe/networkingDB/Server.hpp>

int main() {
    gabe::networkingDB::Server server("databases/server_example.db");
    server.start(18080);

    return 0;
}
