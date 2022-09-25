#include <gabe/networkingDB/Client.hpp>

int main() {
    gabe::networkingDB::Client client1("client_1");
    client1.connect("localhost", 5555);

    return 0;
}
