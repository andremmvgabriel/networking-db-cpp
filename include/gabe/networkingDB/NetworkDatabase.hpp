#pragma once

#include <sqlite3.h>
#include <fmt/format.h>

#include <map>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include <sstream>


typedef std::map<std::string, std::string> row_data_t;
typedef std::vector<row_data_t> table_data_t;


struct insert_res_t {
    const uint64_t id;
    const bool success;

    insert_res_t() : id(0x00), success(false) {};
    insert_res_t(const uint64_t& id) : id(id), success(true) {};
};


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
            void _open_database(const std::string &file_name);

            void _create_sessions_table();
            void _create_clients_table();
            void _create_topics_table();
            void _create_messages_table();

            uint64_t _count_clients();

        public:
            NetworkDatabase();
            NetworkDatabase(const std::string &file_name);
            ~NetworkDatabase();

        public: // Core Functionality
            // Session
            uint64_t open_session();
            void close_session();

            // Clients
            insert_res_t add_client(const uint64_t& session_id, const std::string &name);
            bool disconnect_client(const uint64_t& session_id, const uint64_t &client_id, const std::string &client_name);

            // Topics
            insert_res_t add_topic(const uint64_t &session_id, const uint64_t &client_id, const std::string &name, bool auto_poll);
            bool unsubscribe(const uint64_t &session_id, const uint64_t &client_id, const uint64_t &topic_id);
            bool unsubscribe_all(const uint64_t &session_id, const uint64_t &client_id);

            // Messages
            insert_res_t add_message(const uint64_t &session_id, const uint64_t &topic_id, const std::string &content);
            table_data_t receive_message(const uint64_t &session_id, const uint64_t &topic_id);

        public: // Secondary Functionality - Visualization Methods
            // Sessions
            table_data_t get_sessions() const;
            table_data_t get_session(const uint64_t& session_id) const;
            table_data_t _get_sessions_table_data(const std::string &query) const;

            // Clients
            table_data_t get_clients() const;
            table_data_t get_clients(const uint64_t& session_id) const;
            table_data_t get_client(const uint64_t &session_id) const;
            table_data_t get_client(const uint64_t &session_id, const uint64_t &client_id) const;
            table_data_t _get_clients_table_data(const std::string &query) const;

            // Topics
            table_data_t get_topics() const;
            table_data_t get_topics(const uint64_t& session_id) const;
            table_data_t get_topics_in_client(const uint64_t& client_id) const;
            table_data_t get_topics_in_client(const uint64_t& client_id, const uint64_t& session_id) const;
            table_data_t get_topic(const uint64_t& topic_id) const;
            table_data_t get_topic(const uint64_t& topic_id, const uint64_t& session_id) const;
            table_data_t _get_topics_table_data(const std::string &query) const;

            // Messages
            table_data_t get_messages() const;
            table_data_t get_messages(const uint64_t &session_id) const;
            table_data_t get_messages_in_client(const uint64_t &client_id) const;
            table_data_t get_messages_in_client(const uint64_t &client_id, const uint64_t &session_id) const;
            table_data_t get_messages_in_topic(const uint64_t &topic_id) const;
            table_data_t get_messages_in_topic(const uint64_t &topic_id, const uint64_t &session_id) const;
            table_data_t get_message(const uint64_t &message_id) const;
            table_data_t get_message(const uint64_t &message_id, const uint64_t &session_id) const;
            table_data_t _get_messages_table_data(const std::string &query) const;
        };
    }
}
