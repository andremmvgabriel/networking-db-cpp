#include <gabe/networkingDB/Server.hpp>

gabe::networkingDB::Server::Server() {
    _db.open_session();
    _create_routes();
    _app.port(18080).multithreaded().run();
}

void gabe::networkingDB::Server::_create_routes() {
    // CROW_ROUTE( _app, "/").methods( "GET"_method )(
    //     [&] (const crow::request& request) {}
    // );

    CROW_ROUTE( _app, "/sessions").methods( "GET"_method )(
        [&] (const crow::request& request) {
            std::map<int, std::map<std::string, std::string>> sessions = _db.get_sessions();

            std::string output;

            for (const auto &session : sessions) {
                output += fmt::format("> Session {}: ({})\n", session.first, session.second.at("Timestamp_Close") == "NULL" ? "Active" : "Innactive");
                output += fmt::format("  - Start time: {}\n", session.second.at("Timestamp_Start"));
                output += fmt::format("  - Close time: {}\n", session.second.at("Timestamp_Close"));
                output += fmt::format("  - Connected clients: {}\n", session.second.at("Clients"));
                output += fmt::format("  - Subscribed topics: {}\n", session.second.at("Topics"));
                output += fmt::format("  - Exchanged messages: {}\n", session.second.at("Messages"));
                output += "\n";
            }

            return crow::response(output);
        }
    );

    CROW_ROUTE( _app, "/sessions/<int>").methods( "GET"_method )(
        [&] (const crow::request& request, int session_id) {
            std::map<int, std::map<std::string, std::string>> sessions = _db.get_session(session_id);

            std::string output;

            if (sessions.size()) {
                for (const auto &session : sessions) {
                    output += fmt::format("> Session {}: ({})\n", session_id, session.second.at("Timestamp_Close") == "NULL" ? "Active" : "Innactive");
                    output += fmt::format("  - Start time: {}\n", session.second.at("Timestamp_Start"));
                    output += fmt::format("  - Close time: {}\n", session.second.at("Timestamp_Close"));
                    output += fmt::format("  - Connected clients: {}\n", session.second.at("Clients"));
                    output += fmt::format("  - Subscribed topics: {}\n", session.second.at("Topics"));
                    output += fmt::format("  - Exchanged messages: {}\n", session.second.at("Messages"));
                }
            }
            else {
                output += "Session does not exist.\n";
            }

            return crow::response(output);
        }
    );

    CROW_ROUTE( _app, "/sessions/current").methods( "GET"_method )(
        [&] (const crow::request& request) {
            std::map<int, std::map<std::string, std::string>> sessions = _db.get_current_session();

            std::string output;

            for (const auto &session : sessions) {
                output += fmt::format("> Session {}: ({})\n", session.first, session.second.at("Timestamp_Close") == "NULL" ? "Active" : "Innactive");
                output += fmt::format("  - Start time: {}\n", session.second.at("Timestamp_Start"));
                output += fmt::format("  - Close time: {}\n", session.second.at("Timestamp_Close"));
                output += fmt::format("  - Connected clients: {}\n", session.second.at("Clients"));
                output += fmt::format("  - Subscribed topics: {}\n", session.second.at("Topics"));
                output += fmt::format("  - Exchanged messages: {}\n", session.second.at("Messages"));
            }

            return crow::response(output);
        }
    );

    CROW_ROUTE( _app, "/clients" ).methods( "GET"_method, "POST"_method )(
        [&] (const crow::request& request) {
            if (request.method == "GET"_method) {
                std::map<int, std::map<std::string, std::string>> clients = request.url_params.keys().size() > 0 ? _db.get_clients_in_session(std::stoul(request.url_params.get("session"))) : _db.get_clients();

                std::string output;

                if (clients.size()) {
                    for (const auto &client : clients) {
                        output += fmt::format("> Client {}: ({})\n", client.first, client.second.at("Status"));
                        output += fmt::format("  - Session: {}\n", client.second.at("SID"));
                        output += fmt::format("  - Connection time: {}\n", client.second.at("Timestamp_Conn"));
                        output += fmt::format("  - Disconnection time: {}\n", client.second.at("Timestamp_Disc"));
                        output += fmt::format("  - Name: {}\n", client.second.at("Name"));
                        output += fmt::format("  - Subscribed topics: {}\n", client.second.at("Topics"));
                        output += fmt::format("  - Exchanged messages: {}\n", client.second.at("Messages"));
                        output += "\n";
                    }
                }
                else {
                    output += "There isn't any registry of a connected client in any of the sessions.\n";
                }

                return crow::response(output);
            }
            else if (request.method == "POST"_method) {
                crow::json::wvalue value;

                // TODO: Verification/Validation
                std::string client_name = request.url_params.get("name");

                uint64_t network_id = _db.add_client(client_name);

                value["name"] = client_name;
                value["network_id"] = network_id;

                return crow::response(value);
            }
            else return crow::response(404);
        }
    );

    CROW_ROUTE( _app, "/clients/<int>" ).methods( "GET"_method )(
        [&] (const crow::request& request, int client_id) {
            std::map<int, std::map<std::string, std::string>> clients = _db.get_client(client_id);

            std::string output;

            if (clients.size()) {
                for (const auto &client : clients) {
                    output += fmt::format("> Client {}: ({})\n", client.first, client.second.at("Status"));
                    output += fmt::format("  - Session: {}\n", client.second.at("SID"));
                    output += fmt::format("  - Connection time: {}\n", client.second.at("Timestamp_Conn"));
                    output += fmt::format("  - Disconnection time: {}\n", client.second.at("Timestamp_Disc"));
                    output += fmt::format("  - Name: {}\n", client.second.at("Name"));
                    output += fmt::format("  - Subscribed topics: {}\n", client.second.at("Topics"));
                    output += fmt::format("  - Exchanged messages: {}\n", client.second.at("Messages"));
                    output += "\n";
                }
            }
            else {
                output += "Client does not exist.\n";
            }

            return crow::response(output);
        }
    );

    CROW_ROUTE( _app, "/clients/current" ).methods( "GET"_method )(
        [&] (const crow::request& request) {
            std::map<int, std::map<std::string, std::string>> clients = _db.get_clients_in_current_session();

            std::string output;

            if (clients.size()) {
                for (const auto &client : clients) {
                    output += fmt::format("> Client {}: ({})\n", client.first, client.second.at("Status"));
                    output += fmt::format("  - Session: {}\n", client.second.at("SID"));
                    output += fmt::format("  - Connection time: {}\n", client.second.at("Timestamp_Conn"));
                    output += fmt::format("  - Disconnection time: {}\n", client.second.at("Timestamp_Disc"));
                    output += fmt::format("  - Name: {}\n", client.second.at("Name"));
                    output += fmt::format("  - Subscribed topics: {}\n", client.second.at("Topics"));
                    output += fmt::format("  - Exchanged messages: {}\n", client.second.at("Messages"));
                    output += "\n";
                }
            }
            else {
                output += "The current session does not have any client connected.\n";
            }

            return crow::response(output);
        }
    );

    CROW_ROUTE( _app, "/topics" ).methods( "GET"_method, "POST"_method )(
        [&] (const crow::request& request) {
            if (request.method == "GET"_method) {
                std::map<int, std::map<std::string, std::string>> topics = request.url_params.keys().size() > 0 ? _db.get_topics_in_client(std::stoul(request.url_params.get("client"))) : _db.get_topics();

                std::string output;

                if (topics.size()) {
                    for (const auto &topic : topics) {
                        output += fmt::format("> Topic {}: ({})\n", topic.first, topic.second.at("Status"));
                        output += fmt::format("  - Session: {}\n", topic.second.at("SID"));
                        output += fmt::format("  - Client: {}\n", topic.second.at("CID"));
                        output += fmt::format("  - Subscribed time: {}\n", topic.second.at("Timestamp_Sub"));
                        output += fmt::format("  - Unsubscribed time: {}\n", topic.second.at("Timestamp_Unsub"));
                        output += fmt::format("  - Name: {}\n", topic.second.at("Name"));
                        output += fmt::format("  - Auto poll: {}\n", topic.second.at("Auto_Poll"));
                        output += fmt::format("  - Messages: {}\n", topic.second.at("Messages"));
                        output += "\n";
                    }
                }
                else {
                    output += "There isn't any registry of a subscribed topic in any connected client.\n";
                }

                return crow::response(output);
            }
            else if (request.method == "POST"_method) {
                crow::json::wvalue value;

                // TODO: Verification/Validation
                uint64_t client_id = std::stoul(request.url_params.get("client"));
                std::string topic_name = request.url_params.get("name");
                bool auto_poll = std::stoi(request.url_params.get("auto_poll"));

                uint64_t topic_id = _db.add_topic(client_id, topic_name, auto_poll);

                value["name"] = topic_name;
                value["client"] = client_id;
                value["topic"] = topic_id;

                return crow::response(value);
            }
            else return crow::response(404);
        }
    );

    CROW_ROUTE( _app, "/topics/<int>" ).methods( "GET"_method )(
        [&] (const crow::request& request, int topic_id) {
            std::map<int, std::map<std::string, std::string>> topics = _db.get_topic(topic_id);

            std::string output;

            if (topics.size()) {
                for (const auto &topic : topics) {
                    output += fmt::format("> Topic {}: ({})\n", topic.first, topic.second.at("Status"));
                    output += fmt::format("  - Session: {}\n", topic.second.at("SID"));
                    output += fmt::format("  - Client: {}\n", topic.second.at("CID"));
                    output += fmt::format("  - Subscribed time: {}\n", topic.second.at("Timestamp_Sub"));
                    output += fmt::format("  - Unsubscribed time: {}\n", topic.second.at("Timestamp_Unsub"));
                    output += fmt::format("  - Name: {}\n", topic.second.at("Name"));
                    output += fmt::format("  - Auto poll: {}\n", topic.second.at("Auto_Poll"));
                    output += fmt::format("  - Messages: {}\n", topic.second.at("Messages"));
                    output += "\n";
                }
            }
            else {
                output += "Topic does not exist.\n";
            }

            return crow::response(output);
        }
    );

    CROW_ROUTE( _app, "/topics/current" ).methods( "GET"_method )(
        [&] (const crow::request& request) {
            std::map<int, std::map<std::string, std::string>> topics = _db.get_topics_in_current_session();

            std::string output;

            if (topics.size()) {
                for (const auto &topic : topics) {
                    output += fmt::format("> Topic {}: ({})\n", topic.first, topic.second.at("Status"));
                    output += fmt::format("  - Session: {}\n", topic.second.at("SID"));
                    output += fmt::format("  - Client: {}\n", topic.second.at("CID"));
                    output += fmt::format("  - Subscribed time: {}\n", topic.second.at("Timestamp_Sub"));
                    output += fmt::format("  - Unsubscribed time: {}\n", topic.second.at("Timestamp_Unsub"));
                    output += fmt::format("  - Name: {}\n", topic.second.at("Name"));
                    output += fmt::format("  - Auto poll: {}\n", topic.second.at("Auto_Poll"));
                    output += fmt::format("  - Messages: {}\n", topic.second.at("Messages"));
                    output += "\n";
                }
            }
            else {
                output += "The current session does not have any topic subscribed by any client.\n";
            }

            return crow::response(output);
        }
    );

    CROW_ROUTE( _app, "/messages" ).methods( "GET"_method, "POST"_method )(
        [&] (const crow::request& request) {
            if (request.method == "GET"_method) {
                // std::map<int, std::map<std::string, std::string>> topics = request.url_params.keys().size() > 0 ? _db.get_topics_in_client(std::stoul(request.url_params.get("client"))) : _db.get_topics();
                std::map<int, std::map<std::string, std::string>> messages = _db.get_messages();

                std::string output;

                if (messages.size()) {
                    for (const auto &message : messages) {
                        output += fmt::format("[{}][{}:{}:{}][{}] Message {}: {}\n", message.second.at("Timestamp"), message.second.at("SID"), message.second.at("CID"), message.second.at("TID"), message.second.at("Status"), message.first, message.second.at("Content"));
                    }
                }
                else {
                    output += "There isn't any registry of a message in any topic.\n";
                }

                return crow::response(output);
            }
            else if (request.method == "POST"_method) {
                crow::json::wvalue value;

                // TODO: Verification/Validation
                uint64_t topic_id = std::stoul(request.url_params.get("topic"));
                std::string content = request.url_params.get("content");

                uint64_t message_id = _db.add_message(topic_id, content);

                value["content"] = content;
                value["topic"] = topic_id;
                value["message"] = message_id;

                return crow::response(value);
            }
            else return crow::response(404);
        }
    );

    CROW_ROUTE( _app, "/messages/<int>" ).methods( "GET"_method )(
        [&] (const crow::request& request, int message_id) {
            std::map<int, std::map<std::string, std::string>> messages = _db.get_message(message_id);

            std::string output;

            if (messages.size()) {
                for (const auto &message : messages) {
                    output += fmt::format("[{}][{}:{}:{}][{}] Message {}: {}\n", message.second.at("Timestamp"), message.second.at("SID"), message.second.at("CID"), message.second.at("TID"), message.second.at("Status"), message.first, message.second.at("Content"));
                }
            }
            else {
                output += "The message does not exist.\n";
            }

            return crow::response(output);
        }
    );

    CROW_ROUTE( _app, "/messages/next_pending" ).methods( "GET"_method )(
        [&] (const crow::request& request) {
            uint64_t topic_id = std::stoul(request.url_params.get("topic"));

            std::map<int, std::map<std::string, std::string>> messages = _db.get_next_pending_message(topic_id);

            std::string output;

            if (messages.size()) {
                for (const auto &message : messages) {
                    output += fmt::format("[{}][{}:{}:{}][{}] Message {}: {}\n", message.second.at("Timestamp"), message.second.at("SID"), message.second.at("CID"), message.second.at("TID"), message.second.at("Status"), message.first, message.second.at("Content"));
                }
            }
            else {
                output += "There isn't any message pending in queue.\n";
            }

            return crow::response(output);
        }
    );
}
