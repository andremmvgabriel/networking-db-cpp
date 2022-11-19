#include <gabe/networkingDB/Server.hpp>

gabe::networkingDB::Server::Server() : _db("database.db") {
    _create_sessions_routes();
    _create_clients_routes();
    _create_topics_routes();
    _create_messages_routes();
}

gabe::networkingDB::Server::Server(const std::string &file_name) : _db(file_name) {
    _create_sessions_routes();
    _create_clients_routes();
    _create_topics_routes();
    _create_messages_routes();
}

void gabe::networkingDB::Server::start(const uint16_t &port) {
    _active_session = _db.open_session();
    _app.port(port).multithreaded().run();
}

// Refactored methods

void gabe::networkingDB::Server::_create_sessions_routes() {
    /////////////////////////////////////////////////////////////////////////
    // Sessions
    // -> 
    /////////////////////////////////////////////////////////////////////////
    CROW_ROUTE( _app, "/sessions").methods( "GET"_method )(
        [&] (const crow::request& request) {
            // Gets all the possible input arguments
            auto arg_sid = request.url_params.get("session_id");

            if (request.method == "GET"_method) {
                if (arg_sid != nullptr) {               
                    const uint64_t session_id = std::string(arg_sid) == "current" ? _active_session : std::stoul(arg_sid);

                    table_data_t sessions = _db.get_session(session_id);

                    std::string output = _serialize_sessions_as_readable(sessions);

                    return crow::response(output);
                } else {
                    table_data_t sessions = _db.get_sessions();

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
    CROW_ROUTE( _app, "/clients").methods( "GET"_method, "POST"_method, "DELETE"_method )(
        [&] (const crow::request& request) {
            // Gets all the possible input arguments
            auto arg_sid = request.url_params.get("session_id");
            auto arg_cid = request.url_params.get("client_id");
            auto arg_cname = request.url_params.get("client_name");

            if (request.method == "GET"_method) {
                if (arg_sid != nullptr && arg_cid != nullptr) {
                    if (std::string(arg_sid) == "current") {
                        table_data_t clients = _db.get_client(std::stoul(arg_cid), _active_session);
                        std::string output = _serialize_clients_as_readable(clients);
                        return crow::response(output);
                    }
                } else if (arg_cid != nullptr) {
                    table_data_t clients = _db.get_client(std::stoul(arg_cid));
                    std::string output = _serialize_clients_as_readable(clients);
                    return crow::response(output);
                } else if (arg_sid != nullptr) {
                    const uint64_t session_id = std::string(arg_sid) == "current" ? _active_session : std::stoul(arg_sid);
                    table_data_t clients = _db.get_clients(session_id);
                    std::string output = _serialize_clients_as_readable(clients);
                    return crow::response(output);
                } else {
                    table_data_t clients = _db.get_clients();
                    std::string output = _serialize_clients_as_readable(clients);
                    return crow::response(output);
                }
            }
            else if (request.method == "POST"_method) {
                if (arg_sid != nullptr && arg_cname != nullptr) {
                    if (std::string(arg_sid) == "current") {
                        insert_res_t res = _db.add_client( _active_session, arg_cname );

                        if (res.success) {
                            return crow::response( fmt::format("{}", res.id) );
                        }
                    }
                }
            }
            else if (request.method == "DELETE"_method) {
                if (arg_sid != nullptr && arg_cid != nullptr && arg_cname != nullptr) {
                    if (std::string(arg_sid) == "current") {
                        const uint64_t client_id = std::stoul(arg_cid);

                        bool success = _db.disconnect_client( _active_session, client_id, arg_cname );

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

void gabe::networkingDB::Server::_create_topics_routes() {
    /////////////////////////////////////////////////////////////////////////
    // Topics
    // -> 
    /////////////////////////////////////////////////////////////////////////
    CROW_ROUTE( _app, "/topics").methods( "GET"_method, "POST"_method, "DELETE"_method )(
        [&] (const crow::request& request) {
            // Gets all the possible input arguments
            auto arg_sid = request.url_params.get("session_id");
            auto arg_cid = request.url_params.get("client_id");
            auto arg_tid = request.url_params.get("topic_id");
            auto arg_tname = request.url_params.get("topic_name");
            auto arg_tpoll = request.url_params.get("topic_auto_poll");
            auto arg_unsub_all = request.url_params.get("unsub_all");

            if (request.method == "GET"_method) {
                if(arg_sid && arg_tid) {
                    // Get topic in current session
                    if (std::string(arg_sid) == "current") {
                        table_data_t topics = _db.get_topic(std::stoul(arg_tid), _active_session);
                        std::string output = _serialize_topics_as_readable(topics);
                        return crow::response(output);
                    }
                } else if (arg_sid && arg_cid) {
                    // Get topics in client in current session
                    if (std::string(arg_sid) == "current") {
                        table_data_t topics = _db.get_topics_in_client(std::stoul(arg_cid), _active_session);
                        std::string output = _serialize_topics_as_readable(topics);
                        return crow::response(output);
                    }
                } else if (arg_tid) {
                    // Get topic in lifetime
                    table_data_t topics = _db.get_topic(std::stoul(arg_tid));
                    std::string output = _serialize_topics_as_readable(topics);
                    return crow::response(output);
                } else if (arg_cid) {
                    // Get topics in client in lifetime
                    table_data_t topics = _db.get_topics_in_client(std::stoul(arg_cid));
                    std::string output = _serialize_topics_as_readable(topics);
                    return crow::response(output);
                } else if(arg_sid) {
                    // Get topics in session X
                    // Get topics in current session
                    const uint64_t session_id = std::string(arg_sid) == "current" ? _active_session : std::stoul(arg_sid);
                    table_data_t topics = _db.get_topics(session_id);
                    std::string output = _serialize_topics_as_readable(topics);
                    return crow::response(output);
                } else {
                    // Get topics
                    table_data_t topics = _db.get_topics();
                    std::string output = _serialize_topics_as_readable(topics);
                    return crow::response(output);
                }
            }
            else if (request.method == "POST"_method) {
                if (arg_sid && arg_cid && arg_tname && arg_tpoll) {
                    if (std::string(arg_sid) == "current") {
                        const uint64_t client_id = std::stoul(arg_cid);
                        const int auto_poll = std::stoi(arg_tpoll);

                        insert_res_t res = _db.add_topic( _active_session, client_id, arg_tname, auto_poll );

                        if (res.success) {
                            return crow::response( fmt::format("{}", res.id) );
                        }
                    }
                }
            }
            else if (request.method == "DELETE"_method) {
                if (arg_sid && arg_cid && arg_tid) {
                    if (std::string(arg_sid) == "current") {
                        const uint64_t client_id = std::stoul(arg_cid);
                        const uint64_t topic_id = std::stoul(arg_tid);

                        bool success = _db.unsubscribe( _active_session, client_id, topic_id );

                        if (success) {
                            return crow::response(
                                fmt::format("> Successfully unsubscribed topic {} from client {} in session {}.", topic_id, client_id, _active_session)
                            );
                        }
                    }
                } else if (arg_sid && arg_cid && arg_unsub_all) {
                    if (std::string(arg_sid) == "current" && std::string(arg_unsub_all) == "yes") {
                        const uint64_t client_id = std::stoul(arg_cid);

                        bool success = _db.unsubscribe_all(_active_session, client_id);

                        if (success) {
                            return crow::response(
                                fmt::format("> Successfully unsubscribed all topics from client {} in session {}.", client_id, _active_session)
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

void gabe::networkingDB::Server::_create_messages_routes() {
    /////////////////////////////////////////////////////////////////////////
    // Messages
    // -> 
    /////////////////////////////////////////////////////////////////////////
    CROW_ROUTE( _app, "/messages").methods( "GET"_method, "POST"_method, "DELETE"_method )(
        [&] (const crow::request& request) {
            // Gets all the possible input arguments
            auto arg_sid = request.url_params.get("session_id");
            auto arg_cid = request.url_params.get("client_id");
            auto arg_tid = request.url_params.get("topic_id");
            auto arg_mid = request.url_params.get("message_id");
            auto arg_tname = request.url_params.get("topic_name");
            auto arg_mcontent = request.url_params.get("message_content");

            if (request.method == "GET"_method) {
                if (arg_sid && arg_mid) {
                    // Get message in current session
                    if (std::string(arg_sid) == "current") {
                        const uint64_t message_id = std::stoul(arg_mid);
                        table_data_t messages = _db.get_message(message_id, _active_session);
                        std::string output = _serialize_messages_as_readable(messages);
                        return crow::response(output);
                    }
                } else if (arg_sid && arg_tid) {
                    // Get messages in topic z in current session
                    if (std::string(arg_sid) == "current") {
                        const uint64_t topic_id = std::stoul(arg_tid);
                        table_data_t messages = _db.get_messages_in_topic(topic_id, _active_session);
                        std::string output = _serialize_messages_as_readable(messages);
                        return crow::response(output);
                    }
                } else if (arg_sid && arg_cid) {
                    // Get messages in client y in current session
                    if (std::string(arg_sid) == "current") {
                        const uint64_t client_id = std::stoul(arg_cid);
                        table_data_t messages = _db.get_messages_in_client(client_id, _active_session);
                        std::string output = _serialize_messages_as_readable(messages);
                        return crow::response(output);
                    }
                } else if (arg_mid) {
                    // Get message in lifetime
                    const uint64_t message_id = std::stoul(arg_mid);
                    table_data_t messages = _db.get_message(message_id);
                    std::string output = _serialize_messages_as_readable(messages);
                    return crow::response(output);
                } else if (arg_tid) {
                    // Get messages in topic z in lifetime
                    const uint64_t topic_id = std::stoul(arg_tid);
                    table_data_t messages = _db.get_messages_in_topic(topic_id);
                    std::string output = _serialize_messages_as_readable(messages);
                    return crow::response(output);
                } else if (arg_cid) {
                    // Get messages in client y in lifetime
                    const uint64_t client_id = std::stoul(arg_cid);
                    table_data_t messages = _db.get_messages_in_client(client_id);
                    std::string output = _serialize_messages_as_readable(messages);
                    return crow::response(output);
                } else if (arg_sid) {
                    // Get messages in session x
                    // Get messages in current session
                    const uint64_t session_id = std::string(arg_sid) == "current" ? _active_session : std::stoul(arg_sid);
                    table_data_t messages = _db.get_messages(session_id);
                    std::string output = _serialize_messages_as_readable(messages);
                    return crow::response(output);
                } else {
                    // Get messages in lifetime
                    table_data_t messages = _db.get_messages();
                    std::string output = _serialize_messages_as_readable(messages);
                    return crow::response(output);
                }
            }
            else if (request.method == "POST"_method) {
                if (arg_sid && arg_tid && arg_mcontent) {
                    if (std::string(arg_sid) == "current") {
                        const uint64_t topic_id = std::stoul(arg_tid);

                        insert_res_t res = _db.add_message( _active_session, topic_id, arg_mcontent );

                        if (res.success) {
                            return crow::response( fmt::format("{}", res.id) );
                        }
                    }
                }
            }
            else if (request.method == "DELETE"_method) {
                if (arg_sid && arg_tid) {
                    if (std::string(arg_sid) == "current") {
                        const uint64_t topic_id = std::stoul(arg_tid);

                        table_data_t message = _db.receive_message( _active_session, topic_id );

                        if (!message.empty()) {
                            return crow::response( message.at(0).at("Content") );
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

std::string gabe::networkingDB::Server::_serialize_topics_as_readable(const table_data_t& topics) const {
    std::string serialization;

    if (topics.size()) {
        for (const row_data_t& topic : topics) {
            // Topic header
            serialization += fmt::format(
                "> Topic {}: ({})\n",
                topic.at("ID"), topic.at("Status")
            );

            for (const auto& element : topic) {
                // Topic contents
                serialization += fmt::format("  - {}: {}\n", element.first, element.second);
            }

            serialization += "\n";
        }
    } else {
        serialization = "Requested data is not available.";
    }

    return std::move(serialization);
}

std::string gabe::networkingDB::Server::_serialize_messages_as_readable(const table_data_t& messages) const {
    std::string serialization;

    if (messages.size()) {
        // Info header
        serialization = fmt::format(
            "[{:^13}][{:^8}][{:^10}][{:^5}][{:^5}][{:^5}] {}\n",
            "Timestamp", "Status", "Message ID", "SID", "CID", "TID", "Content"
        );

        for (const row_data_t& message : messages) {
            serialization += fmt::format(
                "[{:13}][{:8}][{:>10}][{:>5}][{:>5}][{:>5}] {}\n",
                message.at("Timestamp"), message.at("Status"), message.at("ID"), message.at("SID"), message.at("CID"), message.at("TID"), message.at("Content") 
            );
        }
    } else {
        serialization = "Requested data is not available.";
    }

    return std::move(serialization);
}
