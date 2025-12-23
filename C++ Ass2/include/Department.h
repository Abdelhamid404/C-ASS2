/*
 * ============================================
 * DEPARTMENT.H - Department Class Definition
 * ============================================
 */

#ifndef DEPARTMENT_H
#define DEPARTMENT_H

#include <string>
#include <vector>
#include "Database.h"

// NOTE: Do NOT use "using namespace std;" in header files as it causes
// conflicts with Windows SDK types like "byte" when used with C++17.

class Department {
private:
    std::string id;
    std::string name;
    std::string collegeId;
    std::string description;

public:
    Department();
    Department(const std::string& id, const std::string& name, 
               const std::string& collegeId, const std::string& description);

    // Getters
    std::string getId() const;
    std::string getName() const;
    std::string getCollegeId() const;
    std::string getDescription() const;

    // Setters
    void setId(const std::string& id);
    void setName(const std::string& name);
    void setCollegeId(const std::string& collegeId);
    void setDescription(const std::string& description);

    // Static database operations
    static std::vector<Department> getAllDepartments(Database& db);
    static Department getById(Database& db, const std::string& id);
    static std::vector<Department> getByCollegeId(Database& db, const std::string& collegeId);
    static bool exists(Database& db, const std::string& id);

    // Instance database operations
    bool save(Database& db);
    bool update(Database& db);
    bool remove(Database& db);

    // Utility methods
    std::string toJson() const;
    bool isEmpty() const;
};

#endif // DEPARTMENT_H
