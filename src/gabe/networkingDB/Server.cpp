#include <gabe/networkingDB/Server.hpp>

gabe::networkingDB::Server::Server() {
    _active_session = _db.open_session();
    _create_routes();
    _create_sessions_routes();
    _create_clients_routes();
    _app.port(18080).multithreaded().run();
}

void gabe::networkingDB::Server::_create_routes() {
    // CROW_ROUTE( _app, "/").methods( "GET"_method )(
    //     [&] (const crow::request& request) {}
    // );

    CROW_ROUTE( _app, "/connect").methods( "GET"_method )(
        [&] (const crow::request& request) {
            // crow::json::wvalue value;

            // TODO: Verification/Validation

            std::string client_name = request.url_params.get("name");

            uint64_t network_id = _db.add_client(client_name);

            // value["name"] = client_name;
            // value["network_id"] = network_id;

            return crow::response(std::to_string(network_id));
        }
    );

    CROW_ROUTE( _app, "/disconnect").methods( "GET"_method )(
        [&] (const crow::request& request) {
            std::string client_name = request.url_params.get("name");
            uint64_t client_id = std::stoul(request.url_params.get("id"));

            _db.disconnect_client(client_id, client_name);

            return crow::response("Disconnected.");
        }
    );

    CROW_ROUTE( _app, "/subscribe").methods( "GET"_method )(
        [&] (const crow::request& request) {
            uint64_t client_id = std::stoul(request.url_params.get("client_id"));
            std::string client_name = request.url_params.get("client_name"); // not being used. Remove if not necessary
            std::string topic = request.url_params.get("topic");
            bool auto_poll = std::stoi(request.url_params.get("auto_poll"));

            uint64_t subscription_id = _db.subscribe(client_id, topic, auto_poll);

            return crow::response(std::to_string(subscription_id));
        }
    );

    CROW_ROUTE( _app, "/unsubscribe").methods( "GET"_method )(
        [&] (const crow::request& request) {
            uint64_t client_id = std::stoul(request.url_params.get("client_id"));
            std::string client_name = request.url_params.get("client_name"); // not being used. Remove if not necessary
            uint64_t topic_id = std::stoul(request.url_params.get("topic_id"));
            std::string topic = request.url_params.get("topic_name");

            _db.unsubscribe(client_id, topic_id);

            return crow::response(200);
        }
    );

    CROW_ROUTE( _app, "/unsubscribe_all").methods( "GET"_method )(
        [&] (const crow::request& request) {
            uint64_t client_id = std::stoul(request.url_params.get("client_id"));
            std::string client_name = request.url_params.get("client_name"); // not being used. Remove if not necessary

            _db.unsubscribe_all(client_id);

            return crow::response(200);
        }
    );

    CROW_ROUTE( _app, "/send_message").methods( "GET"_method )(
        [&] (const crow::request& request) {
            uint64_t client_id = std::stoul(request.url_params.get("client_id"));
            std::string client_name = request.url_params.get("client_name"); // not being used. Remove if not necessary
            uint64_t topic_id = std::stoul(request.url_params.get("topic_id"));
            std::string topic = request.url_params.get("topic_name");
            std::string message = request.url_params.get("message");

            uint64_t message_id = _db.add_message(topic_id, message);

            return crow::response(std::to_string(message_id));
        }
    );

    CROW_ROUTE( _app, "/receive_message").methods( "GET"_method )(
        [&] (const crow::request& request) {
            uint64_t client_id = std::stoul(request.url_params.get("client_id"));
            std::string client_name = request.url_params.get("client_name"); // not being used. Remove if not necessary
            uint64_t topic_id = std::stoul(request.url_params.get("topic_id"));
            std::string topic = request.url_params.get("topic_name");

            std::map<int, std::map<std::string, std::string>> messages = _db.receive_message(topic_id);

            if (messages.size() == 1) {
                for (auto message : messages) {
                    return crow::response(message.second.at("Content"));
                }
            }
            else {
                return crow::response(404);
            }
        }
    );

    //

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

// Refactored methods

void gabe::networkingDB::Server::_create_sessions_routes() {
    /////////////////////////////////////////////////////////////////////////
    // Sessions
    // -> 
    /////////////////////////////////////////////////////////////////////////
    CROW_ROUTE( _app, "/sessions_test").methods( "GET"_method )(
        [&] (const crow::request& request) {
            // Gets all the possible input arguments
            auto arg_sid = request.url_params.get("session_id");

            if (request.method == "GET"_method) {
                if (arg_sid != nullptr) {               
                    const uint64_t session_id = std::string(arg_sid) == "current" ? _active_session : std::stoul(arg_sid);

                    table_data_t sessions = _db.get_session_v2(session_id);

                    std::string output = _serialize_sessions_as_readable(sessions);

                    return crow::response(output);
                } else {
                    table_data_t sessions = _db.get_sessions_v2();

                    std::string output = _serialize_sessions_as_readable(sessions);

                    return crow::response(output);
                }
            }
            // else if (request.method == "POST"_method) {}
            // else if (request.method == "DELETE"_method) {}

            return crow::response(404);
        }
    );
}

void gabe::networkingDB::Server::_create_clients_routes() {
    /////////////////////////////////////////////////////////////////////////
    // Clients
    // -> 
    /////////////////////////////////////////////////////////////////////////
    CROW_ROUTE( _app, "/clients_test").methods( "GET"_method, "POST"_method, "DELETE"_method )(
        [&] (const crow::request& request) {
            // Gets all the possible input arguments
            auto arg_sid = request.url_params.get("session_id");
            auto arg_cid = request.url_params.get("client_id");
            auto arg_cname = request.url_params.get("client_name");

            if (request.method == "GET"_method) {
                if (arg_sid != nullptr) {
                    // const uint64_t session_id = std::string(arg_sid) == "current" ? _active_session : std::stoul(arg_sid);

                    // table_data_t sessions = _db.get_session_v2(session_id);

                    // std::string output = _serialize_sessions_as_readable(sessions);

                    // return crow::response(output);
                } else {
                    table_data_t clients = _db.get_clients_v2();

                    std::string output = _serialize_clients_as_readable(clients);

                    return crow::response(output);
                }
            }
            else if (request.method == "POST"_method) {
                if (arg_sid != nullptr && arg_cname != nullptr) {
                    if (std::string(arg_sid) == "current") {
                        insert_res_t res = _db.add_client_v2( _active_session, arg_cname );

                        if (res.success) {
                            return crow::response(
                                fmt::format("> Successfully created new client with ID {} in session {}.", res.id, _active_session)
                            );
                        }
                    }
                }
            }
            else if (request.method == "DELETE"_method) {
                if (arg_sid != nullptr && arg_cid != nullptr && arg_cname != nullptr) {
                    if (std::string(arg_sid) == "current") {
                        const uint64_t client_id = std::stoul(arg_cid);

                        bool success = _db.disconnect_client_v2( _active_session, client_id, arg_cname );

                        if (success) {
                            return crow::response(
                                fmt::format("> Successfully disconnected client with ID {} in session {}.", client_id, _active_session)
                            );
                        }
                    }
                }
            }

            // Responds with error if no condition is met
            return crow::response(404);
        }
    );
}

std::string gabe::networkingDB::Server::_serialize_sessions_as_readable(const table_data_t& sessions) const {
    std::string serialization;

    if (sessions.size()) {
        for (const row_data_t& session : sessions) {
            // Session header
            serialization += fmt::format(
                "> Session {}: ({})\n",
                session.at("ID"),
                session.at("Timestamp_Close") == "NULL" ? "Active" : "Innactive"
            );

            for (const auto& element : session) {
                // Session contents
                serialization += fmt::format("  - {}: {}\n", element.first, element.second);
            }

            serialization += "\n";
        }
    } else {
        serialization = "Requested data is not available.";
    }

    return std::move(serialization);
}

std::string gabe::networkingDB::Server::_serialize_clients_as_readable(const table_data_t& clients) const {
    std::string serialization;

    if (clients.size()) {
        for (const row_data_t& client : clients) {
            // Client header
            serialization += fmt::format(
                "> Client {}: ({})\n",
                client.at("ID"), client.at("Status")
            );

            for (const auto& element : client) {
                // Client contents
                serialization += fmt::format("  - {}: {}\n", element.first, element.second);
            }

            serialization += "\n";
        }
    } else {
        serialization = "Requested data is not available.";
    }

    return std::move(serialization);
}
