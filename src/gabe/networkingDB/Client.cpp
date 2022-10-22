#include <gabe/networkingDB/Client.hpp>

#include <fmt/format.h>
#include <cpr/cpr.h>

gabe::networkingDB::Client::Client() : name("client") {
    _instances = std::make_shared<uint8_t>(0);
}

gabe::networkingDB::Client::Client(const std::string &name) : name(name) {
    _instances = std::make_shared<uint8_t>(0);
}

gabe::networkingDB::Client::~Client() {
    if (_instances.unique()) disconnect();
}

void gabe::networkingDB::Client::connect(const std::string &host, const uint16_t &port) {
    // Safety check
    if (_network_id) return;

    // Creates the address
    _address = fmt::format("http://{}:{}", host, port);

    cpr::Response response = cpr::Get(
        cpr::Url{_address + "/connect"},
        cpr::Parameters{{"name", name}}
    );

    // printf("Status: %ld\n", response.status_code);
    // printf("Header: %s\n", response.header["content-type"].c_str());
    // printf("Text: %s\n", response.text.c_str());

    if (response.status_code == 200) {
        _network_id = std::stoul(response.text);
        // printf("Client ID: %ld\n", _network_id);
    }
}

void gabe::networkingDB::Client::connect(const std::string &host, const uint16_t &port, const std::string &username, const std::string &password) {
    // Safety check
    if (_network_id) return;

    // Creates the address
    _address = fmt::format("https://{}:{}", host, port);
    _username = username;
    _password = password;

    cpr::Response response = cpr::Get(
        cpr::Url{_address},
        cpr::Authentication{_username, _password, cpr::AuthMode::BASIC}
        // cpr::Parameters{{"name", "name"}}
    );
}

void gabe::networkingDB::Client::disconnect() {
    // TODO: THIS SHOULD NOT BE A GET METHOD
    cpr::Response response = cpr::Get(
        cpr::Url{_address + "/disconnect"},
        cpr::Parameters{{"name", name}, {"id", std::to_string(_network_id)}}
    );
}

void gabe::networkingDB::Client::subscribe(const std::string &topic) {
    // TODO: THIS SHOULD NOT BE A GET METHOD
    cpr::Response response = cpr::Get(
        cpr::Url{_address + "/subscribe"},
        cpr::Parameters{
            {"client_id", std::to_string(_network_id)},
            {"client_name", name},
            {"topic", topic},
            {"auto_poll", "0"}
        }
    );

    // printf("");

    if (response.status_code == 200) { // Everything OK!
        uint64_t topic_id = std::stoul( response.text );
        _topics_map[topic] = topic_id;
    }
}

void gabe::networkingDB::Client::unsubscribe(const std::string &topic) {
    // TODO: THIS SHOULD NOT BE A GET METHOD
    cpr::Response response = cpr::Get(
        cpr::Url{_address + "/unsubscribe"},
        cpr::Parameters{
            {"client_id", std::to_string(_network_id)},
            {"client_name", name},
            {"topic_id", std::to_string(_topics_map.at(topic))},
            {"topic_name", topic}
        }
    );

    if (response.status_code == 200) { // Everything OK!
        _topics_map.erase(topic);
        // REMOVE CALLBACK
    }
}

void gabe::networkingDB::Client::unsubscribe_all() {
    // TODO: THIS SHOULD NOT BE A GET METHOD
    cpr::Response response = cpr::Get(
        cpr::Url{_address + "/unsubscribe_all"},
        cpr::Parameters{
            {"client_id", std::to_string(_network_id)},
            {"client_name", name}
        }
    );

    if (response.status_code == 200) { // Everything OK!
        _topics_map.clear();
        // REMOVE CALLBACK
    }
}

void gabe::networkingDB::Client::send(const std::string &topic, const std::string &message) {
    // TODO: THIS SHOULD NOT BE A GET METHOD
    cpr::Response response = cpr::Get(
        cpr::Url{_address + "/send_message"},
        cpr::Parameters{
            {"client_id", std::to_string(_network_id)},
            {"client_name", name},
            {"topic_id", std::to_string(_topics_map.at(topic))},
            {"topic_name", topic},
            {"message", message}
        }
    );

    if (response.status_code == 200) { // Everything OK!
    }
}

std::string gabe::networkingDB::Client::receive_from(const std::string &topic) {
    // This method should stuck until a response is given (or timeout!)
    
    // TODO: THIS SHOULD NOT BE A GET METHOD
    cpr::Response response = cpr::Get(
        cpr::Url{_address + "/receive_message"},
        cpr::Parameters{
            {"client_id", std::to_string(_network_id)},
            {"client_name", name},
            {"topic_id", std::to_string(_topics_map.at(topic))},
            {"topic_name", topic}
        }
    );

    if (response.status_code == 200) { // Everything OK!
        return response.text;
    }

    return "No message!";
}