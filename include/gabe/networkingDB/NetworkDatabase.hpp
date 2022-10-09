#pragma once

#include <sqlite3.h>
#include <fmt/format.h>

#include <map>
#include <thread>
#include <chrono>
#include <string>
#include <sstream>

namespace gabe {
    namespace networkingDB {
        class NetworkDatabase
        {
        private:
            sqlite3 *_database;
            uint64_t _active_session = 0;
        
        private: // Callbacks
            static int _get_last_inserted_row_id_cb(void *data, int argc, char **argv, char **azColName);
            static int _get_clients_amount_cb(void *data, int argc, char **argv, char **azColName);
            static int _get_sessions_cb(void *data, int argc, char **argv, char **azColName);
            static int _get_clients_cb(void *data, int argc, char **argv, char **azColName);
            static int _get_topics_cb(void *data, int argc, char **argv, char **azColName);
            static int _get_messages_cb(void *data, int argc, char **argv, char **azColName);
        
        private:
            uint64_t _get_thread_id() {
                std::stringstream tid_ss;
                tid_ss << std::this_thread::get_id();
                return std::stoul(tid_ss.str());
            }
        
        private: // Internal functions
            void _open_database();

            void _create_sessions_table();
            void _create_clients_table();
            void _create_topics_table();
            void _create_messages_table();

            uint64_t _count_clients();

        public:
            NetworkDatabase(); // TODO: Add database file name as input
            ~NetworkDatabase();

            uint64_t open_session();
            void close_session();

            std::map<int, std::map<std::string, std::string>> get_sessions();
            std::map<int, std::map<std::string, std::string>> get_session(uint64_t session_id);
            std::map<int, std::map<std::string, std::string>> get_current_session();

            uint64_t add_client(const std::string &name);
            void disconnect_client(const uint64_t &client_id, const std::string &client_name);

            std::map<int, std::map<std::string, std::string>> get_clients();
            std::map<int, std::map<std::string, std::string>> get_client(uint64_t client_id);
            std::map<int, std::map<std::string, std::string>> get_clients_in_session(uint64_t session_id);
            std::map<int, std::map<std::string, std::string>> get_clients_in_current_session();

            uint64_t add_topic(const uint64_t &client_id, const std::string &name, bool auto_poll);

            std::map<int, std::map<std::string, std::string>> get_topics();
            std::map<int, std::map<std::string, std::string>> get_topic(uint64_t topic_id);
            std::map<int, std::map<std::string, std::string>> get_topics_in_client(uint64_t client_id);
            std::map<int, std::map<std::string, std::string>> get_topics_in_current_session();

            uint64_t add_message(const uint64_t &topic_id, const std::string &content);

            std::map<int, std::map<std::string, std::string>> get_messages();
            std::map<int, std::map<std::string, std::string>> get_message(uint64_t message_id);
            std::map<int, std::map<std::string, std::string>> get_next_pending_message(uint64_t topic_id);
        };
    }
}
