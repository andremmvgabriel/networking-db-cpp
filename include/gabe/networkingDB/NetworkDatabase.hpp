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

            static int _get_sessions_cb_v2(void *data, int argc, char **argv, char **azColName);
            static int _get_clients_cb_v2(void *data, int argc, char **argv, char **azColName);
            static int _get_topics_cb_v2(void *data, int argc, char **argv, char **azColName);
            static int _get_messages_cb_v2(void *data, int argc, char **argv, char **azColName);
        
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

        public: // Core Functionality
            // Session
            uint64_t open_session();
            void close_session();

            // Clients
            insert_res_t add_client_v2(const uint64_t& session_id, const std::string &name);
            bool disconnect_client_v2(const uint64_t& session_id, const uint64_t &client_id, const std::string &client_name);

            // Topics
            insert_res_t add_topic_v2(const uint64_t &session_id, const uint64_t &client_id, const std::string &name, bool auto_poll);
            bool unsubscribe_v2(const uint64_t &session_id, const uint64_t &client_id, const uint64_t &topic_id);
            bool unsubscribe_all_v2(const uint64_t &session_id, const uint64_t &client_id);

            // Messages
            insert_res_t add_message_v2(const uint64_t &session_id, const uint64_t &topic_id, const std::string &content);
            table_data_t receive_message_v2(const uint64_t &session_id, const uint64_t &topic_id);

        public: // Secondary Functionality - Visualization Methods
            // Sessions
            table_data_t get_sessions_v2();
            table_data_t get_session_v2(const uint64_t& session_id);
            table_data_t _get_sessions_table_data_v2(const std::string &query);

            // Clients
            table_data_t get_clients_v2();
            table_data_t get_clients_v2(const uint64_t& session_id);
            table_data_t get_client_v2(const uint64_t &session_id);
            table_data_t get_client_v2(const uint64_t &session_id, const uint64_t &client_id);
            table_data_t _get_clients_table_data_v2(const std::string &query);

            // Topics
            table_data_t get_topics_v2() const;
            table_data_t get_topics_v2(const uint64_t& session_id) const;
            table_data_t get_topics_in_client_v2(const uint64_t& client_id) const;
            table_data_t get_topics_in_client_v2(const uint64_t& client_id, const uint64_t& session_id) const;
            table_data_t get_topic_v2(const uint64_t& topic_id) const;
            table_data_t get_topic_v2(const uint64_t& topic_id, const uint64_t& session_id) const;
            table_data_t _get_topics_table_data_v2(const std::string &query) const;

            // Messages
            table_data_t get_messages_v2() const;
            table_data_t get_messages_v2(const uint64_t &session_id) const;
            table_data_t get_messages_in_client_v2(const uint64_t &client_id) const;
            table_data_t get_messages_in_client_v2(const uint64_t &client_id, const uint64_t &session_id) const;
            table_data_t get_messages_in_topic_v2(const uint64_t &topic_id) const;
            table_data_t get_messages_in_topic_v2(const uint64_t &topic_id, const uint64_t &session_id) const;
            table_data_t get_message_v2(const uint64_t &message_id) const;
            table_data_t get_message_v2(const uint64_t &message_id, const uint64_t &session_id) const;
            table_data_t _get_messages_table_data_v2(const std::string &query) const;
        };
    }
}
