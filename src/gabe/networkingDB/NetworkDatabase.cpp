#include <gabe/networkingDB/NetworkDatabase.hpp>

/////////////////////////////////////////////////////////////////////
// Constructor & Destructor
/////////////////////////////////////////////////////////////////////

gabe::networkingDB::NetworkDatabase::NetworkDatabase() {
    _open_database("database.db");
    _create_sessions_table();
    _create_clients_table();
    _create_topics_table();
    _create_messages_table();
}

gabe::networkingDB::NetworkDatabase::NetworkDatabase(const std::string &file_name) {
    _open_database(file_name);
    _create_sessions_table();
    _create_clients_table();
    _create_topics_table();
    _create_messages_table();
}

gabe::networkingDB::NetworkDatabase::~NetworkDatabase() {
    close_session();
    sqlite3_close(_database);
}

/////////////////////////////////////////////////////////////////////
// Internal Functions - Database Creation
/////////////////////////////////////////////////////////////////////

void gabe::networkingDB::NetworkDatabase::_open_database(const std::string &file_name) {
    if (sqlite3_open(file_name.c_str(), &_database) != SQLITE_OK) {
        printf("Database failed to open.\n");
    }
}

void gabe::networkingDB::NetworkDatabase::_create_sessions_table() {
    std::string query = "CREATE TABLE IF NOT EXISTS Sessions            (" \
        "ID                 INTEGER     PRIMARY KEY     AUTOINCREMENT   ," \
        "Timestamp_Start    INTEGER     NOT NULL                        ," \
        "Timestamp_Close    INTEGER                                     ," \
        "Clients            INTEGER     NOT NULL                        ," \
        "Topics             INTEGER     NOT NULL                        ," \
        "Messages           INTEGER     NOT NULL                        );";

    if( sqlite3_exec(_database, &query[0], nullptr, 0, nullptr) != SQLITE_OK ) {
        printf("Failed to create sessions table.\n");
    }
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

    if( sqlite3_exec(_database, &query[0], nullptr, 0, nullptr) != SQLITE_OK ) {
        printf("Failed to create clients table.\n");
    }
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

    if( sqlite3_exec(_database, &query[0], nullptr, 0, nullptr) != SQLITE_OK ) {
        printf("Failed to create topics table.\n");
    }
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

    if( sqlite3_exec(_database, &query[0], nullptr, 0, nullptr) != SQLITE_OK ) {
        printf("Failed to create messages table.\n");
    }
}

uint64_t gabe::networkingDB::NetworkDatabase::_count_clients() {
    // SQL QUERY
    std::string query = fmt::format(
        "SELECT * FROM Clients WHERE ID={};",
        _active_session
    );

    uint64_t number_clients = 0;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_clients_amount_cb, (void*)&number_clients, nullptr) != SQLITE_OK ) {
        printf("Failed to count session clients.\n");
    }

    return number_clients;
}

/////////////////////////////////////////////////////////////////////
// Internal Functions - Callbacks
/////////////////////////////////////////////////////////////////////

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
    table_data_t* output = (table_data_t*)data;

    for (int i = 0; i < argc; i += 6) {
        row_data_t row_data;
        row_data[azColName[i+0]] = argv[i+0] ? argv[i+0] : "NULL";
        row_data[azColName[i+1]] = argv[i+1] ? argv[i+1] : "NULL";
        row_data[azColName[i+2]] = argv[i+2] ? argv[i+2] : "NULL";
        row_data[azColName[i+3]] = argv[i+3] ? argv[i+3] : "NULL";
        row_data[azColName[i+4]] = argv[i+4] ? argv[i+4] : "NULL";
        row_data[azColName[i+5]] = argv[i+5] ? argv[i+5] : "NULL";
        output->push_back(row_data);
    }

    return 0;
}

int gabe::networkingDB::NetworkDatabase::_get_clients_cb(void *data, int argc, char **argv, char **azColName) {
    table_data_t* output = (table_data_t*)data;

    for (int i = 0; i < argc; i += 8) {
        row_data_t row_data;
        row_data[azColName[i+0]] = argv[i+0] ? argv[i+0] : "NULL";
        row_data[azColName[i+1]] = argv[i+1] ? argv[i+1] : "NULL";
        row_data[azColName[i+2]] = argv[i+2] ? argv[i+2] : "NULL";
        row_data[azColName[i+3]] = argv[i+3] ? argv[i+3] : "NULL";
        row_data[azColName[i+4]] = argv[i+4] ? argv[i+4] : "NULL";
        row_data[azColName[i+5]] = argv[i+5] ? argv[i+5] : "NULL";
        row_data[azColName[i+6]] = argv[i+6] ? argv[i+6] : "NULL";
        row_data[azColName[i+7]] = argv[i+7] ? argv[i+7] : "NULL";
        output->push_back(row_data);
    }

    return 0;
}

int gabe::networkingDB::NetworkDatabase::_get_topics_cb(void *data, int argc, char **argv, char **azColName) {
    table_data_t* output = (table_data_t*)data;

    for (int i = 0; i < argc; i += 9) {
        row_data_t row_data;
        row_data[azColName[i+0]] = argv[i+0] ? argv[i+0] : "NULL";
        row_data[azColName[i+1]] = argv[i+1] ? argv[i+1] : "NULL";
        row_data[azColName[i+2]] = argv[i+2] ? argv[i+2] : "NULL";
        row_data[azColName[i+3]] = argv[i+3] ? argv[i+3] : "NULL";
        row_data[azColName[i+4]] = argv[i+4] ? argv[i+4] : "NULL";
        row_data[azColName[i+5]] = argv[i+5] ? argv[i+5] : "NULL";
        row_data[azColName[i+6]] = argv[i+6] ? argv[i+6] : "NULL";
        row_data[azColName[i+7]] = argv[i+7] ? argv[i+7] : "NULL";
        row_data[azColName[i+8]] = argv[i+8] ? argv[i+8] : "NULL";
        output->push_back(row_data);
    }

    return 0;
}

int gabe::networkingDB::NetworkDatabase::_get_messages_cb(void *data, int argc, char **argv, char **azColName) {
    table_data_t* output = (table_data_t*)data;

    for (int i = 0; i < argc; i += 7) {
        row_data_t row_data;
        row_data[azColName[i+0]] = argv[i+0] ? argv[i+0] : "NULL";
        row_data[azColName[i+1]] = argv[i+1] ? argv[i+1] : "NULL";
        row_data[azColName[i+2]] = argv[i+2] ? argv[i+2] : "NULL";
        row_data[azColName[i+3]] = argv[i+3] ? argv[i+3] : "NULL";
        row_data[azColName[i+4]] = argv[i+4] ? argv[i+4] : "NULL";
        row_data[azColName[i+5]] = argv[i+5] ? argv[i+5] : "NULL";
        row_data[azColName[i+6]] = argv[i+6] ? argv[i+6] : "NULL";
        output->push_back(row_data);
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////
// Sessions - Core Functionality
/////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////
// Clients - Core Functionality
/////////////////////////////////////////////////////////////////////

insert_res_t gabe::networkingDB::NetworkDatabase::add_client(const uint64_t &session_id, const std::string &name) {
    // Gets the current timestamp since epoch in milliseconds
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    // SQL QUERY
    std::string query = fmt::format(
        "INSERT INTO Clients (SID,Timestamp_Conn,Status,Name,Topics,Messages) VALUES({},{},'{}','{}',{},{}); SELECT Last_Insert_Rowid();",
        session_id, timestamp, "Active", name, 0, 0
    );

    // Inserted client ID
    uint64_t client_id;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_last_inserted_row_id_cb, (void*)&client_id, nullptr) != SQLITE_OK ) {
        printf("Failed to create new client.\n");
        return insert_res_t();
    }
    
    return insert_res_t(client_id);
}

bool gabe::networkingDB::NetworkDatabase::disconnect_client(const uint64_t &session_id, const uint64_t &client_id, const std::string &client_name) {
    // Gets the current timestamp since epoch in milliseconds
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    // SQL QUERY
    std::string query = fmt::format(
        "UPDATE Clients SET Timestamp_Disc={}, Status='{}' WHERE ID={} AND SID={} AND Name='{}' AND Status='Active';",
        timestamp, "Disconnected", client_id, session_id, client_name
    );

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], nullptr, 0, nullptr) != SQLITE_OK ) {
        printf("Failed to disconnect client.\n");
        return false;
    }
    
    return true;
}

/////////////////////////////////////////////////////////////////////
// Topics - Core Functionality
/////////////////////////////////////////////////////////////////////

insert_res_t gabe::networkingDB::NetworkDatabase::add_topic(const uint64_t &session_id, const uint64_t &client_id, const std::string &name, bool auto_poll) {
    // Gets the current timestamp since epoch in milliseconds
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    // SQL QUERY
    std::string query = fmt::format(
        "INSERT INTO Topics (SID,CID,Timestamp_Sub,Status,Name,Auto_Poll,Messages) VALUES({},{},{},'{}','{}',{},{}); SELECT Last_Insert_Rowid();",
        session_id, client_id, timestamp, "Active", name, auto_poll, 0
    );

    // Inserted topic ID
    uint64_t topic_id;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_last_inserted_row_id_cb, (void*)&topic_id, nullptr) != SQLITE_OK ) {
        printf("Failed to subscribe to topic.\n");
        return insert_res_t();
    }
    
    return insert_res_t(topic_id);
}

bool gabe::networkingDB::NetworkDatabase::unsubscribe(const uint64_t &session_id, const uint64_t &client_id, const uint64_t &topic_id) {
    // Gets the current timestamp since epoch in milliseconds
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    // SQL QUERY
    std::string query = fmt::format(
        "UPDATE Topics SET Timestamp_Unsub={}, Status='{}' WHERE ID={} AND SID={} AND CID={} AND Status='Active';",
        timestamp, "Innactive", topic_id, _active_session, client_id
    );

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], nullptr, 0, nullptr) != SQLITE_OK ) {
        printf("Failed to unsubscribe topic.\n");
        return false;
    }

    return true;
}

bool gabe::networkingDB::NetworkDatabase::unsubscribe_all(const uint64_t &session_id, const uint64_t &client_id) {
    // Gets the current timestamp since epoch in milliseconds
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    // SQL QUERY
    std::string query = fmt::format(
        "UPDATE Topics SET Timestamp_Unsub={}, Status='{}' WHERE SID={} AND CID={} AND Status='Active';",
        timestamp, "Innactive", session_id, client_id
    );

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], nullptr, 0, nullptr) != SQLITE_OK ) {
        printf("Failed to unsubscribe all topics.\n");
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////
// Messages - Core Functionality
/////////////////////////////////////////////////////////////////////

insert_res_t gabe::networkingDB::NetworkDatabase::add_message(const uint64_t &session_id, const uint64_t &topic_id, const std::string &content) {
    // Gets the current timestamp since epoch in milliseconds
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    std::string query = fmt::format(
        "INSERT INTO Messages (SID,CID,TID,Timestamp,Status,Content) VALUES({},(SELECT CID FROM Topics WHERE ID={}),{},{},'{}','{}'); SELECT Last_Insert_Rowid();",
        session_id, topic_id, topic_id, timestamp, "Pending", content
    );

    // Output message ID
    uint64_t message_id;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_last_inserted_row_id_cb, (void*)&message_id, nullptr) != SQLITE_OK ) {
        printf("Failed to create new message.\n");
        return insert_res_t();
    }

    return insert_res_t(message_id);
}

table_data_t gabe::networkingDB::NetworkDatabase::receive_message(const uint64_t &session_id, const uint64_t &topic_id) {
    // SQL QUERY
    std::string query = fmt::format(
        "SELECT * FROM Messages WHERE SID={} AND TID={} AND Status='Pending' ORDER BY ID ASC LIMIT 1; UPDATE Messages SET Status='Received' WHERE ID=(SELECT ID FROM Messages WHERE SID={} AND TID={} AND Status='Pending' ORDER BY ID ASC LIMIT 1) AND SID={} AND TID={} AND EXISTS (SELECT * FROM Messages WHERE SID={} AND TID={} AND Status='Pending' ORDER BY ID ASC LIMIT 1);",
        session_id, topic_id,
        session_id, topic_id,
        session_id, topic_id,
        session_id, topic_id
    );

    // Output
    table_data_t output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_messages_cb, (void*)&output, nullptr) != SQLITE_OK ) {
        printf("Failed to receive message.\n");
    }

    return output;
}

/////////////////////////////////////////////////////////////////////
// Sessions - Visualization Functionality
/////////////////////////////////////////////////////////////////////

table_data_t gabe::networkingDB::NetworkDatabase::get_sessions() {
    const std::string query = "SELECT * FROM Sessions;";
    return _get_sessions_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::get_session(const uint64_t& session_id) {
    const std::string query = fmt::format("SELECT * FROM Sessions WHERE ID = {};", session_id);
    return _get_sessions_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::_get_sessions_table_data(const std::string &query) {
    table_data_t output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_sessions_cb, (void*)&output, nullptr) != SQLITE_OK ) {
        printf("Failed to select sessions info.\n");
    }

    return output;
}

/////////////////////////////////////////////////////////////////////
// Clients - Visualization Functionality
/////////////////////////////////////////////////////////////////////

table_data_t gabe::networkingDB::NetworkDatabase::get_clients() {
    const std::string query = "SELECT * FROM Clients;";
    return _get_clients_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::get_clients(const uint64_t& session_id) {
    const std::string query = fmt::format("SELECT * FROM Clients WHERE SID = {};", session_id);
    return _get_clients_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::get_client(const uint64_t& client_id) {
    const std::string query = fmt::format("SELECT * FROM Clients WHERE ID = {};", client_id);
    return _get_clients_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::get_client(const uint64_t& client_id, const uint64_t& session_id) {
    const std::string query = fmt::format("SELECT * FROM Clients WHERE ID = {} AND SID = {};", client_id, session_id);
    return _get_clients_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::_get_clients_table_data(const std::string &query) {
    table_data_t output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_clients_cb, (void*)&output, nullptr) != SQLITE_OK ) {
        printf("Failed to select clients info.\n");
    }

    return output;
}

/////////////////////////////////////////////////////////////////////
// Topics - Visualization Functionality
/////////////////////////////////////////////////////////////////////

table_data_t gabe::networkingDB::NetworkDatabase::get_topics() const {
    std::string query = "SELECT * FROM Topics;";
    return _get_topics_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::get_topics(const uint64_t &session_id) const {
    std::string query = fmt::format("SELECT * FROM Topics WHERE SID = {};", session_id);
    return _get_topics_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::get_topics_in_client(const uint64_t &client_id) const {
    std::string query = fmt::format("SELECT * FROM Topics WHERE CID = {};", client_id);
    return _get_topics_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::get_topics_in_client(const uint64_t &client_id, const uint64_t &session_id) const {
    std::string query = fmt::format("SELECT * FROM Topics WHERE CID = {} AND SID = {};", client_id, session_id);
    return _get_topics_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::get_topic(const uint64_t &topic_id) const {
    std::string query = fmt::format("SELECT * FROM Topics WHERE ID = {};", topic_id);
    return _get_topics_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::get_topic(const uint64_t &topic_id, const uint64_t &session_id) const {
    std::string query = fmt::format("SELECT * FROM Topics WHERE ID = {} AND SID = {};", topic_id, session_id);
    return _get_topics_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::_get_topics_table_data(const std::string &query) const {
    table_data_t output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_topics_cb, (void*)&output, nullptr) != SQLITE_OK ) {
        printf("Failed to select topics info.\n");
    }

    return output;
}

/////////////////////////////////////////////////////////////////////
// Messages - Visualization Functionality
/////////////////////////////////////////////////////////////////////

table_data_t gabe::networkingDB::NetworkDatabase::get_messages() const {
    std::string query = "SELECT * FROM Messages;";
    return _get_messages_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::get_messages(const uint64_t &session_id) const {
    std::string query = fmt::format("SELECT * FROM Messages WHERE SID = {};", session_id);
    return _get_messages_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::get_messages_in_client(const uint64_t &client_id) const {
    std::string query = fmt::format("SELECT * FROM Messages WHERE CID = {};", client_id);
    return _get_messages_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::get_messages_in_client(const uint64_t &client_id, const uint64_t &session_id) const {
    std::string query = fmt::format("SELECT * FROM Messages WHERE CID = {} AND SID = {};", client_id, session_id);
    return _get_messages_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::get_messages_in_topic(const uint64_t &topic_id) const {
    std::string query = fmt::format("SELECT * FROM Messages WHERE TID = {};", topic_id);
    return _get_messages_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::get_messages_in_topic(const uint64_t &topic_id, const uint64_t &session_id) const {
    std::string query = fmt::format("SELECT * FROM Messages WHERE TID = {} AND SID = {};", topic_id, session_id);
    return _get_messages_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::get_message(const uint64_t &message_id) const {
    std::string query = fmt::format("SELECT * FROM Messages WHERE ID = {};", message_id);
    return _get_messages_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::get_message(const uint64_t &message_id, const uint64_t &session_id) const {
    std::string query = fmt::format("SELECT * FROM Messages WHERE ID = {} AND SID = {};", message_id, session_id);
    return _get_messages_table_data(query);
}

table_data_t gabe::networkingDB::NetworkDatabase::_get_messages_table_data(const std::string &query) const {
    table_data_t output;

    // SQL QUERY Execution
    if( sqlite3_exec(_database, &query[0], _get_messages_cb, (void*)&output, nullptr) != SQLITE_OK ) {
        printf("Failed to select messages info.\n");
    }

    return output;
}
