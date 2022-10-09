#include <gabe/networkingDB/NetworkDatabase.hpp>

gabe::networkingDB::NetworkDatabase::NetworkDatabase() {
    // _open_database();
    // _create_sessions_table();
    // uint64_t session_id = _create_new_session();
    // _create_session_clients_table();
    // uint64_t client_id1 = add_client("client");
    // uint64_t client_id2 = add_client("client");
    // uint64_t client_id3 = add_client("client");
    // uint64_t client_id4 = add_client("client");
    // uint64_t client_id5 = add_client("client");
    // _create_session_topics_table();
    // _create_new_subscription(client_id1, "topic1");
    // _create_new_subscription(client_id1, "topic2");
    // _create_new_subscription(client_id1, "topic3");
    // _create_new_subscription(client_id2, "topic4");
    // _create_new_subscription(client_id2, "topic5");
    // _create_new_subscription(client_id5, "topic6");

    // get_sessions();
    // get_session(1);
    // get_message();


    _open_database();
    _create_sessions_table();\
    _create_clients_table();
    _create_topics_table();
    _create_messages_table();
}

gabe::networkingDB::NetworkDatabase::~NetworkDatabase() {
    close_session();
    sqlite3_close(_database);
}

// Public methods

uint64_t gabe::networkingDB::NetworkDatabase::open_session() {
    // Gets the current timestamp since epoch in milliseconds
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    // SQL QUERY
    std::string query = fmt::format(
        "INSERT INTO Sessions (Timestamp_Start, Clients, Topics, Messages) VALUES({},{},{},{}); SELECT Last_Insert_Rowid();",
        timestamp, 0, 0, 0
    );

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_last_inserted_row_id_cb, (void*)&_active_session, nullptr) != SQLITE_OK ) {
        printf("Failed to create new session.\n");
    }

    return _active_session;
}

void gabe::networkingDB::NetworkDatabase::close_session() {
    // Gets the current timestamp since epoch in milliseconds
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    // SQL QUERY
    std::string query = fmt::format(
        "UPDATE Sessions SET Timestamp_Close={}, Clients={} WHERE ID={};",
        timestamp,
        _count_clients(),
        _active_session
    );

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], nullptr, 0, nullptr) != SQLITE_OK )
        printf("Failed to close the session.\n");
}

std::map<int, std::map<std::string, std::string>> gabe::networkingDB::NetworkDatabase::get_sessions() {
    // SQL QUERY
    std::string query = "SELECT * FROM Sessions;";

    // Output
    std::map<int, std::map<std::string, std::string>> output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_sessions_cb, (void*)&output, nullptr) != SQLITE_OK )
        printf("Failed to select sessions info.\n");

    return output;
}

std::map<int, std::map<std::string, std::string>> gabe::networkingDB::NetworkDatabase::get_session(uint64_t session_id) {
    // SQL QUERY
    std::string query = fmt::format("SELECT * FROM Sessions WHERE ID = {};", session_id);

    // Output variable
    std::map<int, std::map<std::string, std::string>> output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_sessions_cb, (void*)&output, nullptr) != SQLITE_OK )
        printf("Failed to select session info.\n");
    
    return output;
}

std::map<int, std::map<std::string, std::string>> gabe::networkingDB::NetworkDatabase::get_current_session() {
    return get_session(_active_session);
}

uint64_t gabe::networkingDB::NetworkDatabase::add_client(const std::string &name) {
    // Gets the current timestamp since epoch in milliseconds
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    // SQL QUERY
    std::string query = fmt::format(
        "INSERT INTO Clients (SID,Timestamp_Conn,Status,Name,Topics,Messages) VALUES({},{},'{}','{}',{},{}); SELECT Last_Insert_Rowid();",
        _active_session, timestamp, "Active", name, 0, 0
    );

    // Output client ID
    uint64_t client_id;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_last_inserted_row_id_cb, (void*)&client_id, nullptr) != SQLITE_OK )
        printf("Failed to create new client.\n");
    
    return client_id;
}

std::map<int, std::map<std::string, std::string>> gabe::networkingDB::NetworkDatabase::get_clients() {
    // SQL QUERY
    std::string query = "SELECT * FROM Clients;";

    // Output
    std::map<int, std::map<std::string, std::string>> output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_clients_cb, (void*)&output, nullptr) != SQLITE_OK )
        printf("Failed to select clients info.\n");

    return output;
}

std::map<int, std::map<std::string, std::string>> gabe::networkingDB::NetworkDatabase::get_client(uint64_t client_id) {
    // SQL QUERY
    std::string query = fmt::format("SELECT * FROM Clients WHERE ID = {};", client_id);

    // Output variable
    std::map<int, std::map<std::string, std::string>> output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_clients_cb, (void*)&output, nullptr) != SQLITE_OK )
        printf("Failed to select client info.\n");
    
    return output;
}

std::map<int, std::map<std::string, std::string>> gabe::networkingDB::NetworkDatabase::get_clients_in_session(uint64_t session_id) {
    // SQL QUERY
    std::string query = fmt::format("SELECT * FROM Clients WHERE SID={};", session_id);

    // Output
    std::map<int, std::map<std::string, std::string>> output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_clients_cb, (void*)&output, nullptr) != SQLITE_OK )
        printf("Failed to select clients info.\n");

    return output;
}

std::map<int, std::map<std::string, std::string>> gabe::networkingDB::NetworkDatabase::get_clients_in_current_session() {
    // SQL QUERY
    std::string query = fmt::format("SELECT * FROM Clients WHERE SID={};", _active_session);

    // Output
    std::map<int, std::map<std::string, std::string>> output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_clients_cb, (void*)&output, nullptr) != SQLITE_OK )
        printf("Failed to select clients info.\n");

    return output;
}

uint64_t gabe::networkingDB::NetworkDatabase::add_topic(const uint64_t &client_id, const std::string &name, bool auto_poll) {
    // Gets the current timestamp since epoch in milliseconds
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    // SQL QUERY
    std::string query = fmt::format(
        "INSERT INTO Topics (SID,CID,Timestamp_Sub,Status,Name,Auto_Poll,Messages) VALUES({},{},{},'{}','{}',{},{}); SELECT Last_Insert_Rowid();",
        _active_session, client_id, timestamp, "Active", name, auto_poll, 0
    );

    // Output topic ID
    uint64_t topic_id;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_last_inserted_row_id_cb, (void*)&topic_id, nullptr) != SQLITE_OK )
        printf("Failed to create new topic.\n");
    
    return topic_id;
}

std::map<int, std::map<std::string, std::string>> gabe::networkingDB::NetworkDatabase::get_topics() {
    // SQL QUERY
    std::string query = "SELECT * FROM Topics;";

    // Output
    std::map<int, std::map<std::string, std::string>> output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_topics_cb, (void*)&output, nullptr) != SQLITE_OK )
        printf("Failed to select topics info.\n");

    return output;
}

std::map<int, std::map<std::string, std::string>> gabe::networkingDB::NetworkDatabase::get_topic(uint64_t topic_id) {
    // SQL QUERY
    std::string query = fmt::format("SELECT * FROM Topics WHERE ID={};", topic_id);

    // Output
    std::map<int, std::map<std::string, std::string>> output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_topics_cb, (void*)&output, nullptr) != SQLITE_OK )
        printf("Failed to select topic info.\n");

    return output;
}

std::map<int, std::map<std::string, std::string>> gabe::networkingDB::NetworkDatabase::get_topics_in_client(uint64_t client_id) {
    // SQL QUERY
    std::string query = fmt::format("SELECT * FROM Topics WHERE CID={};", client_id);

    // Output
    std::map<int, std::map<std::string, std::string>> output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_topics_cb, (void*)&output, nullptr) != SQLITE_OK )
        printf("Failed to select topics info.\n");

    return output;
}

std::map<int, std::map<std::string, std::string>> gabe::networkingDB::NetworkDatabase::get_topics_in_current_session() {
    // SQL QUERY
    std::string query = fmt::format("SELECT * FROM Topics WHERE SID={};", _active_session);

    // Output
    std::map<int, std::map<std::string, std::string>> output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_topics_cb, (void*)&output, nullptr) != SQLITE_OK )
        printf("Failed to select topics info.\n");

    return output;
}

uint64_t gabe::networkingDB::NetworkDatabase::add_message(const uint64_t &topic_id, const std::string &content) {
    // Gets the current timestamp since epoch in milliseconds
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    // SQL QUERY
    // std::string query = fmt::format(
    //     "INSERT INTO Messages (SID,CID,TID,Timestamp,Status,Content) VALUES({},{},{},{},'{}','{}'); SELECT Last_Insert_Rowid();",
    //     _active_session, client_id, timestamp, "Active", name, auto_poll, 0
    // );

    std::string query = fmt::format(
        "INSERT INTO Messages (SID,CID,TID,Timestamp,Status,Content) VALUES({},(SELECT CID FROM Topics WHERE ID={}),{},{},'{}','{}'); SELECT Last_Insert_Rowid();",
        _active_session, topic_id, topic_id, timestamp, "Pending", content
    );

    // Output message ID
    uint64_t message_id;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_last_inserted_row_id_cb, (void*)&message_id, nullptr) != SQLITE_OK )
        printf("Failed to create new message.\n");
    
    return message_id;
}

std::map<int, std::map<std::string, std::string>> gabe::networkingDB::NetworkDatabase::get_messages() {
    // SQL QUERY
    std::string query = "SELECT * FROM Messages;";

    // Output
    std::map<int, std::map<std::string, std::string>> output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_messages_cb, (void*)&output, nullptr) != SQLITE_OK )
        printf("Failed to select messages info.\n");

    return output;
}

std::map<int, std::map<std::string, std::string>> gabe::networkingDB::NetworkDatabase::get_message(uint64_t message_id) {
    // SQL QUERY
    std::string query = fmt::format("SELECT * FROM Messages WHERE ID={};", message_id);

    // Output
    std::map<int, std::map<std::string, std::string>> output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_messages_cb, (void*)&output, nullptr) != SQLITE_OK )
        printf("Failed to select messages info.\n");

    return output;
}

std::map<int, std::map<std::string, std::string>> gabe::networkingDB::NetworkDatabase::get_next_pending_message(uint64_t topic_id) {
    // SQL QUERY
    std::string query = fmt::format(
        "SELECT * FROM Messages WHERE TID={} AND Status='Pending' ORDER BY ID ASC LIMIT 1; UPDATE Messages SET Status='Received' WHERE ID=(SELECT ID FROM Messages WHERE TID={} AND Status='Pending' ORDER BY ID ASC LIMIT 1) AND TID={} AND EXISTS (SELECT * FROM Messages WHERE TID={} AND Status='Pending' ORDER BY ID ASC LIMIT 1);", topic_id, topic_id, topic_id, topic_id);

    // Output
    std::map<int, std::map<std::string, std::string>> output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_messages_cb, (void*)&output, nullptr) != SQLITE_OK )
        printf("Failed to select next pending message.\n");

    return output;
}
















// static int _temp(void *data, int argc, char **argv, char **azColName) {
//     for (int i = 0 ; i < argc; i++) {
//         printf("%s: %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//     }

//     return 0;
// }

// std::string gabe::networkingDB::NetworkDatabase::get_message() {
//     std::string output;

//     // std::string query = "UPDATE Sessions SET CLIENTS=registry.CLIENTS+1 FROM ( SELECT ID, CLIENTS FROM Sessions WHERE ID = 1 ORDER BY ID ASC LIMIT 1 ) WHERE ID = 1";
//     // std::string query = "UPDATE Sessions SET CLIENTS=1 FROM ( SELECT ID, CLIENTS FROM Sessions WHERE ID = 1 ) AS registry WHERE ID = 1;";
//     // std::string query = "UPDATE Sessions SET CLIENTS=1 FROM SELECT * FROM Session_1 AS registry WHERE ID=1;";
//     // std::string query = "UPDATE Sessions SET CLIENTS=100 WHERE EXISTS ( SELECT ID, CLIENTS FROM Sessions WHERE ID = 100 ) AND ID = 1";

//     // if( sqlite3_exec(_database, &query[0], _temp, (void*)&output, nullptr) != SQLITE_OK )
//     //     printf("Failed to get message.\n");

//     return output;
// }

// Internal functions

void gabe::networkingDB::NetworkDatabase::_open_database() {
    if (sqlite3_open("database.db", &_database) != SQLITE_OK)
        printf("Database failed to open.\n");
}

void gabe::networkingDB::NetworkDatabase::_create_sessions_table() {
    std::string query = "CREATE TABLE IF NOT EXISTS Sessions            (" \
        "ID                 INTEGER     PRIMARY KEY     AUTOINCREMENT   ," \
        "Timestamp_Start    INTEGER     NOT NULL                        ," \
        "Timestamp_Close    INTEGER                                     ," \
        "Clients            INTEGER     NOT NULL                        ," \
        "Topics             INTEGER     NOT NULL                        ," \
        "Messages           INTEGER     NOT NULL                        );";

    if( sqlite3_exec(_database, &query[0], nullptr, 0, nullptr) != SQLITE_OK )
        printf("Failed to create sessions table.\n");
}

void gabe::networkingDB::NetworkDatabase::_create_clients_table() {    
    std::string query = "CREATE TABLE IF NOT EXISTS Clients             (" \
        "ID                 INTEGER     PRIMARY KEY     AUTOINCREMENT   ," \
        "SID                INTEGER     NOT NULL                        ," \
        "Timestamp_Conn     INTEGER     NOT NULL                        ," \
        "Timestamp_Disc     INTEGER                                     ," \
        "Status             INTEGER     NOT NULL                        ," \
        "Name               Text        NOT NULL                        ," \
        "Topics             INTEGER     NOT NULL                        ," \
        "Messages           INTEGER     NOT NULL                        ," \
        "FOREIGN KEY (SID)  REFERENCES  Sessions(ID)                    );";

    if( sqlite3_exec(_database, &query[0], nullptr, 0, nullptr) != SQLITE_OK )
        printf("Failed to create clients table.\n");
}

void gabe::networkingDB::NetworkDatabase::_create_topics_table() {    
    std::string query = "CREATE TABLE IF NOT EXISTS Topics              (" \
        "ID                 INTEGER     PRIMARY KEY     AUTOINCREMENT   ," \
        "SID                INTEGER     NOT NULL                        ," \
        "CID                INTEGER     NOT NULL                        ," \
        "Timestamp_Sub      INTEGER     NOT NULL                        ," \
        "Timestamp_Unsub    INTEGER                                     ," \
        "Status             TEXT        NOT NULL                        ," \
        "Name               TEXT        NOT NULL                        ," \
        "Auto_Poll          INTEGER     NOT NULL                        ," \
        "Messages           INTEGER     NOT NULL                        ," \
        "FOREIGN KEY (SID)  REFERENCES  Sessions(ID)                    ," \
        "FOREIGN KEY (CID)  REFERENCES  Clients(ID)                     );";

    if( sqlite3_exec(_database, &query[0], nullptr, 0, nullptr) != SQLITE_OK )
        printf("Failed to create topics table.\n");
}

void gabe::networkingDB::NetworkDatabase::_create_messages_table() {        
    std::string query = "CREATE TABLE IF NOT EXISTS Messages            (" \
        "ID                 INTEGER     PRIMARY KEY     AUTOINCREMENT   ," \
        "SID                INTEGER     NOT NULL                        ," \
        "CID                INTEGER     NOT NULL                        ," \
        "TID                INTEGER     NOT NULL                        ," \
        "Timestamp          INTEGER     NOT NULL                        ," \
        "Status             INTEGER     NOT NULL                        ," \
        "Content            Text        NOT NULL                        ," \
        "FOREIGN KEY (SID)  REFERENCES  Sessions(ID)                    ," \
        "FOREIGN KEY (CID)  REFERENCES  Clients(ID)                     ," \
        "FOREIGN KEY (TID)  REFERENCES  Topics(ID)                      );";

    if( sqlite3_exec(_database, &query[0], nullptr, 0, nullptr) != SQLITE_OK )
        printf("Failed to create messages table.\n");
}

uint64_t gabe::networkingDB::NetworkDatabase::_count_clients() {
    // SQL QUERY
    std::string query = fmt::format(
        "SELECT * FROM Clients WHERE ID={};",
        _active_session
    );

    uint64_t number_clients = 0;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_clients_amount_cb, (void*)&number_clients, nullptr) != SQLITE_OK )
        printf("Failed to count session clients.\n");

    return number_clients;
}

// Callbacks

int gabe::networkingDB::NetworkDatabase::_get_last_inserted_row_id_cb(void *data, int argc, char **argv, char **azColName) {
    uint64_t* row_id = (uint64_t*)data;
    *row_id = std::stoul(argv[0]);
    return 0;
}

int gabe::networkingDB::NetworkDatabase::_get_clients_amount_cb(void *data, int argc, char **argv, char **azColName) {
    uint64_t* counter = (uint64_t*)data;
    *counter = atoi(argv[0]);
    return 0;
}

int gabe::networkingDB::NetworkDatabase::_get_sessions_cb(void *data, int argc, char **argv, char **azColName) {
    std::map<int, std::map<std::string, std::string>>* output = (std::map<int, std::map<std::string, std::string>>*)data;

    for (int i = 0; i < argc; i += 6) {
        std::map<std::string, std::string> map;
        map[azColName[i+1]] = argv[i+1] ? argv[i+1] : "NULL";
        map[azColName[i+2]] = argv[i+2] ? argv[i+2] : "NULL";
        map[azColName[i+3]] = argv[i+3] ? argv[i+3] : "NULL";
        map[azColName[i+4]] = argv[i+4] ? argv[i+4] : "NULL";
        map[azColName[i+5]] = argv[i+5] ? argv[i+5] : "NULL";
        output->insert(std::pair<int, std::map<std::string, std::string>>(std::stoi(argv[i]), map));
    }

    return 0;
}

int gabe::networkingDB::NetworkDatabase::_get_clients_cb(void *data, int argc, char **argv, char **azColName) {
    std::map<int, std::map<std::string, std::string>>* output = (std::map<int, std::map<std::string, std::string>>*)data;

    for (int i = 0; i < argc; i += 8) {
        std::map<std::string, std::string> map;
        map[azColName[i+1]] = argv[i+1] ? argv[i+1] : "NULL";
        map[azColName[i+2]] = argv[i+2] ? argv[i+2] : "NULL";
        map[azColName[i+3]] = argv[i+3] ? argv[i+3] : "NULL";
        map[azColName[i+4]] = argv[i+4] ? argv[i+4] : "NULL";
        map[azColName[i+5]] = argv[i+5] ? argv[i+5] : "NULL";
        map[azColName[i+6]] = argv[i+6] ? argv[i+6] : "NULL";
        map[azColName[i+7]] = argv[i+7] ? argv[i+7] : "NULL";
        output->insert(std::pair<int, std::map<std::string, std::string>>(std::stoi(argv[i]), map));
    }

    return 0;
}

int gabe::networkingDB::NetworkDatabase::_get_topics_cb(void *data, int argc, char **argv, char **azColName) {
    std::map<int, std::map<std::string, std::string>>* output = (std::map<int, std::map<std::string, std::string>>*)data;

    for (int i = 0; i < argc; i += 9) {
        std::map<std::string, std::string> map;
        map[azColName[i+1]] = argv[i+1] ? argv[i+1] : "NULL";
        map[azColName[i+2]] = argv[i+2] ? argv[i+2] : "NULL";
        map[azColName[i+3]] = argv[i+3] ? argv[i+3] : "NULL";
        map[azColName[i+4]] = argv[i+4] ? argv[i+4] : "NULL";
        map[azColName[i+5]] = argv[i+5] ? argv[i+5] : "NULL";
        map[azColName[i+6]] = argv[i+6] ? argv[i+6] : "NULL";
        map[azColName[i+7]] = argv[i+7] ? argv[i+7] : "NULL";
        map[azColName[i+8]] = argv[i+8] ? argv[i+8] : "NULL";
        output->insert(std::pair<int, std::map<std::string, std::string>>(std::stoi(argv[i]), map));
    }

    return 0;
}

int gabe::networkingDB::NetworkDatabase::_get_messages_cb(void *data, int argc, char **argv, char **azColName) {
    std::map<int, std::map<std::string, std::string>>* output = (std::map<int, std::map<std::string, std::string>>*)data;

    for (int i = 0; i < argc; i += 7) {
        std::map<std::string, std::string> map;
        map[azColName[i+1]] = argv[i+1] ? argv[i+1] : "NULL";
        map[azColName[i+2]] = argv[i+2] ? argv[i+2] : "NULL";
        map[azColName[i+3]] = argv[i+3] ? argv[i+3] : "NULL";
        map[azColName[i+4]] = argv[i+4] ? argv[i+4] : "NULL";
        map[azColName[i+5]] = argv[i+5] ? argv[i+5] : "NULL";
        map[azColName[i+6]] = argv[i+6] ? argv[i+6] : "NULL";
        output->insert(std::pair<int, std::map<std::string, std::string>>(std::stoi(argv[i]), map));
    }

    return 0;
}
