#pragma once

#include <sqlite3.h>
#include <fmt/format.h>

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
        
        private:
            static int _insert_callback(void *data, int argc, char **argv, char **azColName) {
                uint64_t* row_id = (uint64_t*)data;
                *row_id = std::stoul(argv[0]);
                return 0;
            }

            static int _clients_counter_callback(void *data, int argc, char **argv, char **azColName) {
                uint64_t* counter = (uint64_t*)data;
                *counter = atoi(argv[0]);
                return 0;
            }
        
        private:
            void _open_database() {
                if (sqlite3_open("database.db", &_database) != SQLITE_OK)
                    printf("Database failed to open\n");
            }

            void _close_database() {
                _close_session();
            }

            uint64_t _get_thread_id() {
                std::stringstream tid_ss;
                tid_ss << std::this_thread::get_id();
                return std::stoul(tid_ss.str());
            }

            void _create_sessions_table() {
                std::string query = "CREATE TABLE IF NOT EXISTS Sessions("\
                    "ID             INTEGER     PRIMARY KEY     AUTOINCREMENT,"\
                    "TIMESTAMP_ON   INTEGER     NOT NULL,"\
                    "TIMESTAMP_OFF  INTEGER,"\
                    "CLIENTS        INTEGER,"\
                    "MESSAGES       INTEGER);";

                if( sqlite3_exec(_database, &query[0], nullptr, 0, nullptr) != SQLITE_OK )
                    printf("Failed to create sessions table.\n");
            }

            uint64_t _create_new_session() {
                uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

                std::string query = fmt::format(
                    "INSERT INTO Sessions (TIMESTAMP_ON) VALUES({:d}); SELECT Last_Insert_Rowid();",
                    timestamp
                );

                uint64_t session_id;

                if( sqlite3_exec(_database, &query[0], _insert_callback, (void*)&session_id, nullptr) != SQLITE_OK )
                    printf("Failed to create new session.\n");
                
                _active_session = session_id;
                
                return session_id;
            }

            void _create_session_clients_table() {
                std::string query = fmt::format(
                    "CREATE TABLE IF NOT EXISTS Session_{:d}_clients("\
                    "ID             INTEGER     PRIMARY KEY     AUTOINCREMENT,"\
                    "TID            INTEGER     NOT NULL,"\
                    "NAME           TEXT        NOT NULL,"\
                    "ACTIVE         INTEGER     NOT NULL,"\
                    "TIMESTAMP_ON   INTEGER,"\
                    "TIMESTAMP_OFF  INTEGER,"\
                    "MESSAGES_SENT  INTEGER,"\
                    "MESSAGES_RECV  INTEGER);",
                    _active_session
                );

                if( sqlite3_exec(_database, &query[0], nullptr, 0, nullptr) != SQLITE_OK )
                    printf("Failed to create clients session table.\n");
            }

            uint64_t _create_new_client(const std::string &name) {
                uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

                std::string query = fmt::format(
                    "INSERT INTO Session_{:d}_clients (TID,NAME,ACTIVE) VALUES({:d},'{:s}',{:d}); SELECT Last_Insert_Rowid();",
                    _active_session,
                    _get_thread_id(),
                    name,
                    (int)true
                );

                uint64_t client_id;

                if( sqlite3_exec(_database, &query[0], _insert_callback, (void*)&client_id, nullptr) != SQLITE_OK )
                    printf("Failed to create new client.\n");
                
                return client_id;
            }

            uint64_t _count_session_clients() {
                std::string query = fmt::format(
                    "SELECT * FROM Session_{:d}_clients;",
                    _active_session
                );

                uint64_t number_clients = 0;

                if( sqlite3_exec(_database, &query[0], _clients_counter_callback, (void*)&number_clients, nullptr) != SQLITE_OK )
                    printf("Failed to count session clients.\n");

                return number_clients;
            }

            void _create_session_topics_table() {
                std::string query = fmt::format(
                    "CREATE TABLE IF NOT EXISTS Session_{:d}_topics("\
                    "ID             INTEGER     PRIMARY KEY     AUTOINCREMENT,"\
                    "CID            INTEGER     NOT NULL,"\
                    "NAME           TEXT        NOT NULL,"\
                    "AUTO_POLL      INTEGER     NOT NULL,"\
                    "MESSAGES       INTEGER);",
                    _active_session
                );

                if( sqlite3_exec(_database, &query[0], nullptr, 0, nullptr) != SQLITE_OK )
                    printf("Failed to create topics session table.\n");
            }

            uint64_t _create_new_subscription(const uint64_t &client_id, const std::string &name) {
                uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

                std::string query = fmt::format(
                    "INSERT INTO Session_{:d}_topics (CID,NAME,AUTO_POLL) VALUES({:d},'{:s}',{:d}); SELECT Last_Insert_Rowid();",
                    _active_session,
                    client_id,
                    name,
                    (int)true
                );

                uint64_t subscription_id;

                if( sqlite3_exec(_database, &query[0], _insert_callback, (void*)&subscription_id, nullptr) != SQLITE_OK )
                    printf("Failed to create new subscription.\n");
                
                return subscription_id;
            }

            void _close_session() {
                uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

                std::string query = fmt::format(
                    "UPDATE Sessions SET TIMESTAMP_OFF = {:d}, CLIENTS = {:d} WHERE ID = {:d};",
                    timestamp,
                    _count_session_clients(),
                    _active_session
                );

                if( sqlite3_exec(_database, &query[0], nullptr, 0, nullptr) != SQLITE_OK )
                    printf("Failed to close session.\n");
            }

        public:
            NetworkDatabase() {
                _open_database();
                _create_sessions_table();
                uint64_t session_id = _create_new_session();
                _create_session_clients_table();
                uint64_t client_id1 = _create_new_client("client");
                uint64_t client_id2 = _create_new_client("client");
                uint64_t client_id3 = _create_new_client("client");
                uint64_t client_id4 = _create_new_client("client");
                uint64_t client_id5 = _create_new_client("client");
                _create_session_topics_table();
                _create_new_subscription(client_id1, "topic1");
                _create_new_subscription(client_id1, "topic2");
                _create_new_subscription(client_id1, "topic3");
                _create_new_subscription(client_id2, "topic4");
                _create_new_subscription(client_id2, "topic5");
                _create_new_subscription(client_id5, "topic6");
            }

            ~NetworkDatabase() {
                _close_database();
            }

            void open() {}
        };
    }
}
