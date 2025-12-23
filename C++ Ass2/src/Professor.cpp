/*
 * ============================================
 * PROFESSOR.CPP - Professor Class Implementation
 * ============================================
 */

#include "../include/Professor.h"
#include <sstream>

using namespace std;

// ========================================
// CONSTRUCTORS
// ========================================

Professor::Professor() 
    : id(""), userId(""), firstName(""), lastName(""), email(""),
      phone(""), departmentId(""), hireDate(""),
      specialization(""), passwordHash("prof123") {
}

Professor::Professor(const string& id, const string& firstName, 
                     const string& lastName, const string& email,
                     const string& phone, const string& departmentId,
                     const string& specialization)
    : id(id), userId(""), firstName(firstName), lastName(lastName), 
      email(email), phone(phone), departmentId(departmentId),
      specialization(specialization), passwordHash("prof123") {
}

// ========================================
// GETTERS
// ========================================

string Professor::getId() const { return id; }
string Professor::getUserId() const { return userId; }
string Professor::getFirstName() const { return firstName; }
string Professor::getLastName() const { return lastName; }
string Professor::getFullName() const { return firstName + " " + lastName; }
string Professor::getEmail() const { return email; }
string Professor::getPhone() const { return phone; }
string Professor::getDepartmentId() const { return departmentId; }
string Professor::getHireDate() const { return hireDate; }
string Professor::getSpecialization() const { return specialization; }

// ========================================
// SETTERS
// ========================================

void Professor::setId(const string& id) { this->id = id; }
void Professor::setUserId(const string& userId) { this->userId = userId; }
void Professor::setFirstName(const string& firstName) { this->firstName = firstName; }
void Professor::setLastName(const string& lastName) { this->lastName = lastName; }
void Professor::setEmail(const string& email) { this->email = email; }
void Professor::setPhone(const string& phone) { this->phone = phone; }
void Professor::setDepartmentId(const string& departmentId) { this->departmentId = departmentId; }
void Professor::setSpecialization(const string& specialization) { this->specialization = specialization; }
void Professor::setPassword(const string& password) { this->passwordHash = password; }

// ========================================
// STATIC DATABASE OPERATIONS
// ========================================

vector<Professor> Professor::getAllProfessors(Database& db) {
    vector<Professor> professors;
    
    try {
        auto result = db.executeQuery(
            "SELECT p.id, p.user_id, p.first_name, p.last_name, u.email, u.phone, "
            "p.department_id, p.hire_date, p.specialization "
            "FROM professors p "
            "LEFT JOIN users u ON p.user_id = u.id "
            "ORDER BY p.id"
        );
        
        while (result->next()) {
            Professor prof;
            prof.id = result->getString("id");
            prof.userId = result->getString("user_id");
            prof.firstName = result->getString("first_name");
            prof.lastName = result->getString("last_name");
            prof.email = result->getString("email");
            prof.phone = result->getString("phone");
            prof.departmentId = result->getString("department_id");
            prof.hireDate = result->getString("hire_date");
            prof.specialization = result->getString("specialization");
            
            professors.push_back(prof);
        }
        
    } catch (exception& e) {
        cerr << "[Professor] Error getting all professors: " << e.what() << endl;
    }
    
    return professors;
}

Professor Professor::getById(Database& db, const string& id) {
    Professor prof;
    
    try {
        auto stmt = db.prepareStatement(
            "SELECT p.id, p.user_id, p.first_name, p.last_name, u.email, u.phone, "
            "p.department_id, p.hire_date, p.specialization "
            "FROM professors p "
            "LEFT JOIN users u ON p.user_id = u.id "
            "WHERE p.id = ?"
        );
        stmt->setString(1, id);
        
        auto result = stmt->executeQuery();
        
        if (result->next()) {
            prof.id = result->getString("id");
            prof.userId = result->getString("user_id");
            prof.firstName = result->getString("first_name");
            prof.lastName = result->getString("last_name");
            prof.email = result->getString("email");
            prof.phone = result->getString("phone");
            prof.departmentId = result->getString("department_id");
            prof.hireDate = result->getString("hire_date");
            prof.specialization = result->getString("specialization");
        }
        
    } catch (exception& e) {
        cerr << "[Professor] Error getting professor by ID: " << e.what() << endl;
    }
    
    return prof;
}

bool Professor::exists(Database& db, const string& id) {
    try {
        auto stmt = db.prepareStatement("SELECT COUNT(*) as count FROM professors WHERE id = ?");
        stmt->setString(1, id);
        
        auto result = stmt->executeQuery();
        
        if (result->next()) {
            return result->getInt("count") > 0;
        }
        
    } catch (exception& e) {
        cerr << "[Professor] Error checking if professor exists: " << e.what() << endl;
    }
    
    return false;
}

vector<Professor> Professor::getByDepartment(Database& db, const string& departmentId) {
    vector<Professor> professors;
    
    try {
        auto stmt = db.prepareStatement(
            "SELECT p.id, p.user_id, p.first_name, p.last_name, u.email, u.phone, "
            "p.department_id, p.hire_date, p.specialization "
            "FROM professors p "
            "LEFT JOIN users u ON p.user_id = u.id "
            "WHERE p.department_id = ? ORDER BY p.id"
        );
        stmt->setString(1, departmentId);
        
        auto result = stmt->executeQuery();
        
        while (result->next()) {
            Professor prof;
            prof.id = result->getString("id");
            prof.userId = result->getString("user_id");
            prof.firstName = result->getString("first_name");
            prof.lastName = result->getString("last_name");
            prof.email = result->getString("email");
            prof.phone = result->getString("phone");
            prof.departmentId = result->getString("department_id");
            prof.hireDate = result->getString("hire_date");
            prof.specialization = result->getString("specialization");
            
            professors.push_back(prof);
        }
        
    } catch (exception& e) {
        cerr << "[Professor] Error getting professors by department: " << e.what() << endl;
    }
    
    return professors;
}

Professor Professor::validateLogin(Database& db, const string& id, const string& password) {
    Professor prof;
    
    try {
        auto stmt = db.prepareStatement(
            "SELECT p.id, p.user_id, p.first_name, p.last_name, u.email, u.phone, "
            "p.department_id, p.hire_date, p.specialization "
            "FROM users u "
            "JOIN professors p ON p.user_id = u.id "
            "WHERE u.username = ? AND u.password_hash = ? AND u.is_active = TRUE AND u.role_id = 'ROLE_PROFESSOR'"
        );
        stmt->setString(1, id);
        stmt->setString(2, password);
        
        auto result = stmt->executeQuery();
        
        if (result->next()) {
            prof.id = result->getString("id");
            prof.userId = result->getString("user_id");
            prof.firstName = result->getString("first_name");
            prof.lastName = result->getString("last_name");
            prof.email = result->getString("email");
            prof.phone = result->getString("phone");
            prof.departmentId = result->getString("department_id");
            prof.hireDate = result->getString("hire_date");
            prof.specialization = result->getString("specialization");
        }
        
    } catch (exception& e) {
        cerr << "[Professor] Error validating login: " << e.what() << endl;
    }
    
    return prof;
}

// ========================================
// INSTANCE DATABASE OPERATIONS
// ========================================

bool Professor::save(Database& db) {
    if (exists(db, this->id)) {
        cerr << "[Professor] Error: Professor ID '" << this->id << "' already exists!" << endl;
        return false;
    }
    
    try {
        db.executeUpdate("START TRANSACTION");

        // Prevent duplicate usernames
        auto userCheck = db.prepareStatement("SELECT COUNT(*) as cnt FROM users WHERE username = ?");
        userCheck->setString(1, id);
        auto userCheckResult = userCheck->executeQuery();
        if (userCheckResult->next() && userCheckResult->getInt("cnt") > 0) {
            db.executeUpdate("ROLLBACK");
            cerr << "[Professor] Error: Username '" << id << "' already exists!" << endl;
            return false;
        }

        // Create user (RBAC)
        auto userStmt = db.prepareStatement(
            "INSERT INTO users (username, password_hash, full_name, email, phone, role_id) "
            "VALUES (?, ?, ?, ?, ?, 'ROLE_PROFESSOR')"
        );
        userStmt->setString(1, id);
        userStmt->setString(2, passwordHash);
        userStmt->setString(3, getFullName());
        if (email.empty()) userStmt->setNull(4); else userStmt->setString(4, email);
        if (phone.empty()) userStmt->setNull(5); else userStmt->setString(5, phone);
        userStmt->executeUpdate();

        userId = to_string(db.getLastInsertId());

        // Create professor profile
        auto stmt = db.prepareStatement(
            "INSERT INTO professors (id, user_id, first_name, last_name, department_id, specialization) "
            "VALUES (?, ?, ?, ?, ?, ?)"
        );
        stmt->setString(1, id);
        stmt->setString(2, userId);
        stmt->setString(3, firstName);
        stmt->setString(4, lastName);
        if (departmentId.empty()) stmt->setNull(5); else stmt->setString(5, departmentId);
        if (specialization.empty()) stmt->setNull(6); else stmt->setString(6, specialization);
        stmt->executeUpdate();

        db.executeUpdate("COMMIT");
        
        cout << "[Professor] Successfully added professor: " << id << endl;
        return true;
        
    } catch (exception& e) {
        try { db.executeUpdate("ROLLBACK"); } catch (...) {}
        cerr << "[Professor] Error saving professor: " << e.what() << endl;
        return false;
    }
}

bool Professor::update(Database& db) {
    try {
        auto userIdStmt = db.prepareStatement("SELECT user_id FROM professors WHERE id = ?");
        userIdStmt->setString(1, id);
        auto userIdResult = userIdStmt->executeQuery();
        if (!userIdResult->next()) {
            cerr << "[Professor] No professor found with ID: " << id << endl;
            return false;
        }
        userId = userIdResult->getString("user_id");

        db.executeUpdate("START TRANSACTION");

        auto stmt = db.prepareStatement(
            "UPDATE professors SET first_name = ?, last_name = ?, department_id = ?, specialization = ? "
            "WHERE id = ?"
        );
        
        stmt->setString(1, firstName);
        stmt->setString(2, lastName);
        if (departmentId.empty()) stmt->setNull(3); else stmt->setString(3, departmentId);
        if (specialization.empty()) stmt->setNull(4); else stmt->setString(4, specialization);
        stmt->setString(5, id);
        stmt->executeUpdate();

        if (!userId.empty()) {
            auto userStmt = db.prepareStatement(
                "UPDATE users SET full_name = ?, email = ?, phone = ? WHERE id = ?"
            );
            userStmt->setString(1, getFullName());
            if (email.empty()) userStmt->setNull(2); else userStmt->setString(2, email);
            if (phone.empty()) userStmt->setNull(3); else userStmt->setString(3, phone);
            userStmt->setString(4, userId);
            userStmt->executeUpdate();
        }

        // MySQL may report 0 affected rows if values are unchanged; treat as success.
        db.executeUpdate("COMMIT");
        cout << "[Professor] Successfully updated professor: " << id << endl;
        return true;
        
    } catch (exception& e) {
        try { db.executeUpdate("ROLLBACK"); } catch (...) {}
        cerr << "[Professor] Error updating professor: " << e.what() << endl;
        return false;
    }
}

bool Professor::remove(Database& db) {
    try {
        if (userId.empty()) {
            auto userIdStmt = db.prepareStatement("SELECT user_id FROM professors WHERE id = ?");
            userIdStmt->setString(1, id);
            auto userIdResult = userIdStmt->executeQuery();
            if (userIdResult->next()) {
                userId = userIdResult->getString("user_id");
            }
        }

        db.executeUpdate("START TRANSACTION");

        auto stmt = db.prepareStatement("DELETE FROM professors WHERE id = ?");
        stmt->setString(1, id);
        int rowsAffected = stmt->executeUpdate();

        if (rowsAffected > 0 && !userId.empty()) {
            auto userStmt = db.prepareStatement("DELETE FROM users WHERE id = ?");
            userStmt->setString(1, userId);
            userStmt->executeUpdate();
        }
        
        if (rowsAffected > 0) {
            db.executeUpdate("COMMIT");
            cout << "[Professor] Successfully deleted professor: " << id << endl;
            return true;
        }

        db.executeUpdate("ROLLBACK");
        return false;
        
    } catch (exception& e) {
        try { db.executeUpdate("ROLLBACK"); } catch (...) {}
        cerr << "[Professor] Error deleting professor: " << e.what() << endl;
        return false;
    }
}

// ========================================
// UTILITY METHODS
// ========================================

string Professor::toJson() const {
    stringstream ss;
    ss << "{";
    ss << "\"id\":\"" << id << "\",";
    ss << "\"firstName\":\"" << firstName << "\",";
    ss << "\"lastName\":\"" << lastName << "\",";
    ss << "\"fullName\":\"" << getFullName() << "\",";
    ss << "\"email\":\"" << email << "\",";
    ss << "\"phone\":\"" << phone << "\",";
    ss << "\"departmentId\":\"" << departmentId << "\",";
    ss << "\"hireDate\":\"" << hireDate << "\",";
    ss << "\"specialization\":\"" << specialization << "\"";
    ss << "}";
    return ss.str();
}

bool Professor::isEmpty() const {
    return id.empty();
}
