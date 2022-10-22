#include <gabe/networkingDB/Client.hpp>
#include <unistd.h>
#include <stdio.h>

int main() {
    gabe::networkingDB::Client client1("client_1");

    client1.connect("localhost", 18080);

    client1.subscribe("topic_1");
    client1.subscribe("topic_2");
    client1.subscribe("topic_3");
    client1.subscribe("topic_4");
    client1.subscribe("topic_5");

    client1.send("topic_1", "This is my first message!");

    client1.unsubscribe("topic_3");

    sleep(1);

    std::string topic1_msg = client1.receive_from("topic_1");
    std::string topic2_msg = client1.receive_from("topic_2");

    printf("Topic 1 message: %s\n", topic1_msg.c_str());
    printf("Topic 2 message: %s\n", topic2_msg.c_str());

    std::string topic1_msg2 = client1.receive_from("topic_1");
    printf("Topic 1 message: %s\n", topic1_msg2.c_str());

    sleep(1);
    
    client1.unsubscribe_all();

    return 0;
}
