/*
 * ============================================
 * DATABASE.H - MySQL Database Connection
 * ============================================
 * Uses MySQL C API (libmysql)
 */

#ifndef DATABASE_H
#define DATABASE_H

// MySQL C API header location differs between platforms/installations:
// - Windows typically provides: <mysql.h>
// - Linux typically provides:   <mysql/mysql.h>
// If MySQL is installed via vcpkg or MySQL Connector/C, use:
//   <mysql/mysql.h>
// If using MySQL Server's include folder directly, use:
//   <mysql.h>
#if defined(_WIN32)
// Try mysql.h first, fall back to mysql/mysql.h
#if __has_include(<mysql.h>)
#include <mysql.h>
#elif __has_include(<mysql/mysql.h>)
#include <mysql/mysql.h>
#else
// Fallback - user needs to configure include path
#include <mysql.h>
#endif
#else
#include <mysql/mysql.h>
#endif

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>

/*
 * ResultSet - Wrapper for MySQL result set
 */
class ResultSet {
private:
    MYSQL_RES* result;
    MYSQL_ROW currentRow;
    MYSQL_FIELD* fields;
    unsigned int numFields;
    
public:
    ResultSet(MYSQL_RES* res) : result(res), currentRow(nullptr), fields(nullptr), numFields(0) {
        if (result) {
            fields = mysql_fetch_fields(result);
            numFields = mysql_num_fields(result);
        }
    }
    
    ~ResultSet() {
        if (result) {
            mysql_free_result(result);
        }
    }
    
    bool next() {
        if (!result) return false;
        currentRow = mysql_fetch_row(result);
        return currentRow != nullptr;
    }
    
    int getFieldIndex(const std::string& fieldName) const {
        for (unsigned int i = 0; i < numFields; i++) {
            if (fields[i].name == fieldName) {
                return i;
            }
        }
        return -1;
    }
    
    std::string getString(const std::string& fieldName) const {
        int idx = getFieldIndex(fieldName);
        if (idx >= 0 && currentRow && currentRow[idx]) {
            return std::string(currentRow[idx]);
        }
        return "";
    }
    
    std::string getString(int index) const {
        if (currentRow && index >= 0 && index < (int)numFields && currentRow[index]) {
            return std::string(currentRow[index]);
        }
        return "";
    }
    
    int getInt(const std::string& fieldName) const {
        std::string val = getString(fieldName);
        return val.empty() ? 0 : std::stoi(val);
    }
    
    int getInt(int index) const {
        std::string val = getString(index);
        return val.empty() ? 0 : std::stoi(val);
    }
    
    double getDouble(const std::string& fieldName) const {
        std::string val = getString(fieldName);
        return val.empty() ? 0.0 : std::stod(val);
    }
    
    bool getBoolean(const std::string& fieldName) const {
        return getInt(fieldName) != 0;
    }
    
    bool isNull(const std::string& fieldName) const {
        int idx = getFieldIndex(fieldName);
        if (idx >= 0 && currentRow) {
            return currentRow[idx] == nullptr;
        }
        return true;
    }
    
    bool isNull(int index) const {
        if (currentRow && index >= 0 && index < (int)numFields) {
            return currentRow[index] == nullptr;
        }
        return true;
    }
};

/*
 * PreparedStatement - Simple prepared statement wrapper
 */
class PreparedStatement {
private:
    MYSQL* conn;
    std::string query;
    std::vector<std::string> params;
    int paramCount;
    
public:
    PreparedStatement(MYSQL* connection, const std::string& sql) 
        : conn(connection), query(sql), paramCount(0) {
        // Count placeholders
        for (char c : sql) {
            if (c == '?') paramCount++;
        }
        params.assign(paramCount, "NULL");
    }

    void setString(int index, const std::string& value) {
        if (index > 0 && index <= paramCount) {
            // Escape the string
            char* escaped = new char[value.length() * 2 + 1];
            mysql_real_escape_string(conn, escaped, value.c_str(), (unsigned long)value.length());
            params[index - 1] = "'" + std::string(escaped) + "'";
            delete[] escaped;
        }
    }

    void setNull(int index) {
        if (index > 0 && index <= paramCount) {
            params[index - 1] = "NULL";
        }
    }

    void setInt(int index, int value) {
        if (index > 0 && index <= paramCount) {
            params[index - 1] = std::to_string(value);
        }
    }
    
    void setDouble(int index, double value) {
        if (index > 0 && index <= paramCount) {
            params[index - 1] = std::to_string(value);
        }
    }
    
    std::string buildQuery() const {
        std::string result;
        int paramIdx = 0;
        for (char c : query) {
            if (c == '?' && paramIdx < paramCount) {
                result += params[paramIdx++];
            } else {
                result += c;
            }
        }
        return result;
    }
    
    std::unique_ptr<ResultSet> executeQuery() {
        std::string finalQuery = buildQuery();
        if (mysql_query(conn, finalQuery.c_str()) != 0) {
            throw std::runtime_error(std::string("Query error: ") + mysql_error(conn));
        }
        MYSQL_RES* res = mysql_store_result(conn);
        return std::make_unique<ResultSet>(res);
    }
    
    int executeUpdate() {
        std::string finalQuery = buildQuery();
        if (mysql_query(conn, finalQuery.c_str()) != 0) {
            throw std::runtime_error(std::string("Query error: ") + mysql_error(conn));
        }
        return (int)mysql_affected_rows(conn);
    }
};

/*
 * Database - Main database connection class
 */
class Database {
private:
    MYSQL* connection;
    std::string host;
    std::string user;
    std::string password;
    std::string database;
    int port;

public:
    // Constructor with default connection parameters
    Database() 
        : connection(nullptr),
          host("localhost"),
          user("root"),
          password("admin"),
          database("nctu_sis"),
          port(3306) {
    }
    
    // Constructor with custom parameters
    Database(const std::string& host, const std::string& user, 
             const std::string& password, const std::string& database, int port = 3306)
        : connection(nullptr),
          host(host),
          user(user),
          password(password),
          database(database),
          port(port) {
    }
    
    // Destructor
    ~Database() {
        disconnect();
    }
    
    // Connect to database
    bool connect() {
        if (connection) return true;  // Already connected
        
        connection = mysql_init(nullptr);
        if (!connection) {
            std::cerr << "[Database] Failed to initialize MySQL" << std::endl;
            return false;
        }
        
        if (!mysql_real_connect(connection, host.c_str(), user.c_str(), 
                                password.c_str(), database.c_str(), 
                                port, nullptr, 0)) {
            std::cerr << "[Database] Connection failed: " << mysql_error(connection) << std::endl;
            mysql_close(connection);
            connection = nullptr;
            return false;
        }
        
        // Set UTF-8 encoding
        mysql_set_character_set(connection, "utf8mb4");
        
        std::cout << "[Database] Connected to " << database << " on " << host << std::endl;
        return true;
    }
    
    // Disconnect from database
    void disconnect() {
        if (connection) {
            mysql_close(connection);
            connection = nullptr;
            std::cout << "[Database] Disconnected" << std::endl;
        }
    }
    
    // Test connection
    bool testConnection() {
        if (connect()) {
            std::cout << "[Database] Connection test successful!" << std::endl;
            return true;
        }
        return false;
    }
    
    // Check if connected
    bool isConnected() const {
        return connection != nullptr;
    }
    
    // Execute a query and return results
    std::unique_ptr<ResultSet> executeQuery(const std::string& query) {
        if (!connect()) {
            throw std::runtime_error("Not connected to database");
        }
        
        if (mysql_query(connection, query.c_str()) != 0) {
            throw std::runtime_error(std::string("Query error: ") + mysql_error(connection));
        }
        
        MYSQL_RES* res = mysql_store_result(connection);
        return std::make_unique<ResultSet>(res);
    }
    
    // Execute an update (INSERT, UPDATE, DELETE)
    int executeUpdate(const std::string& query) {
        if (!connect()) {
            throw std::runtime_error("Not connected to database");
        }
        
        if (mysql_query(connection, query.c_str()) != 0) {
            throw std::runtime_error(std::string("Query error: ") + mysql_error(connection));
        }
        
        return (int)mysql_affected_rows(connection);
    }
    
    // Prepare a statement
    std::unique_ptr<PreparedStatement> prepareStatement(const std::string& query) {
        if (!connect()) {
            throw std::runtime_error("Not connected to database");
        }
        return std::make_unique<PreparedStatement>(connection, query);
    }
    
    // Get last insert ID
    unsigned long long getLastInsertId() const {
        if (connection) {
            return mysql_insert_id(connection);
        }
        return 0;
    }
    
    // Escape string for SQL
    std::string escapeString(const std::string& str) {
        if (!connection) return str;
        char* escaped = new char[str.length() * 2 + 1];
        mysql_real_escape_string(connection, escaped, str.c_str(), (unsigned long)str.length());
        std::string result(escaped);
        delete[] escaped;
        return result;
    }
};

#endif // DATABASE_H
