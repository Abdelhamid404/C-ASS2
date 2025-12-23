/*
 * ============================================
 * COLLEGE.CPP - College Class Implementation
 * ============================================
 */

#include "../include/College.h"
#include <sstream>

using namespace std;

// Constructors
College::College() : id(""), name(""), description("") {}

College::College(const string& id, const string& name, const string& description)
    : id(id), name(name), description(description) {}

// Getters
string College::getId() const { return id; }
string College::getName() const { return name; }
string College::getDescription() const { return description; }

// Setters
void College::setId(const string& id) { this->id = id; }
void College::setName(const string& name) { this->name = name; }
void College::setDescription(const string& description) { this->description = description; }

// Static database operations
vector<College> College::getAllColleges(Database& db) {
    vector<College> colleges;
    
    try {
        auto result = db.executeQuery("SELECT id, name, description FROM colleges ORDER BY name");
        
        while (result->next()) {
            College college;
            college.id = result->getString("id");
            college.name = result->getString("name");
            college.description = result->getString("description");
            colleges.push_back(college);
        }
    } catch (exception& e) {
        cerr << "[College] Error: " << e.what() << endl;
    }
    
    return colleges;
}

College College::getById(Database& db, const string& id) {
    College college;
    
    try {
        auto stmt = db.prepareStatement("SELECT id, name, description FROM colleges WHERE id = ?");
        stmt->setString(1, id);
        auto result = stmt->executeQuery();
        
        if (result->next()) {
            college.id = result->getString("id");
            college.name = result->getString("name");
            college.description = result->getString("description");
        }
    } catch (exception& e) {
        cerr << "[College] Error: " << e.what() << endl;
    }
    
    return college;
}

bool College::exists(Database& db, const string& id) {
    try {
        auto stmt = db.prepareStatement("SELECT COUNT(*) as count FROM colleges WHERE id = ?");
        stmt->setString(1, id);
        auto result = stmt->executeQuery();
        if (result->next()) return result->getInt("count") > 0;
    } catch (exception& e) {
        cerr << "[College] Error: " << e.what() << endl;
    }
    return false;
}

// Instance database operations
bool College::save(Database& db) {
    if (exists(db, this->id)) return false;
    
    try {
        auto stmt = db.prepareStatement(
            "INSERT INTO colleges (id, name, description) VALUES (?, ?, ?)"
        );
        stmt->setString(1, id);
        stmt->setString(2, name);
        stmt->setString(3, description);
        stmt->executeUpdate();
        return true;
    } catch (exception& e) {
        cerr << "[College] Error: " << e.what() << endl;
        return false;
    }
}

bool College::update(Database& db) {
    try {
        auto stmt = db.prepareStatement(
            "UPDATE colleges SET name = ?, description = ? WHERE id = ?"
        );
        stmt->setString(1, name);
        stmt->setString(2, description);
        stmt->setString(3, id);
        return stmt->executeUpdate() > 0;
    } catch (exception& e) {
        cerr << "[College] Error: " << e.what() << endl;
        return false;
    }
}

bool College::remove(Database& db) {
    try {
        auto stmt = db.prepareStatement("DELETE FROM colleges WHERE id = ?");
        stmt->setString(1, id);
        return stmt->executeUpdate() > 0;
    } catch (exception& e) {
        cerr << "[College] Error: " << e.what() << endl;
        return false;
    }
}

string College::toJson() const {
    stringstream ss;
    ss << "{\"id\":\"" << id << "\",\"name\":\"" << name 
       << "\",\"description\":\"" << description << "\"}";
    return ss.str();
}

bool College::isEmpty() const { return id.empty(); }
