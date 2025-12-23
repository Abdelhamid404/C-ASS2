/*
 * ============================================
 * COLLEGE.H - College Class Definition
 * ============================================
 */

#ifndef COLLEGE_H
#define COLLEGE_H

#include <string>
#include <vector>
#include "Database.h"

// NOTE: Do NOT use "using namespace std;" in header files as it causes
// conflicts with Windows SDK types like "byte" when used with C++17.

class College {
private:
    std::string id;
    std::string name;
    std::string description;

public:
    College();
    College(const std::string& id, const std::string& name, const std::string& description);

    // Getters
    std::string getId() const;
    std::string getName() const;
    std::string getDescription() const;

    // Setters
    void setId(const std::string& id);
    void setName(const std::string& name);
    void setDescription(const std::string& description);

    // Static database operations
    static std::vector<College> getAllColleges(Database& db);
    static College getById(Database& db, const std::string& id);
    static bool exists(Database& db, const std::string& id);

    // Instance database operations
    bool save(Database& db);
    bool update(Database& db);
    bool remove(Database& db);

    // Utility methods
    std::string toJson() const;
    bool isEmpty() const;
};

#endif // COLLEGE_H
