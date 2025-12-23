/*
 * ============================================
 * ACADEMICLEVEL.H - Academic Level Class Definition
 * ============================================
 */

#ifndef ACADEMICLEVEL_H
#define ACADEMICLEVEL_H

#include <string>
#include <vector>
#include "Database.h"

// NOTE: Do NOT use "using namespace std;" in header files as it causes
// conflicts with Windows SDK types like "byte" when used with C++17.

class AcademicLevel {
private:
    std::string id;
    std::string name;
    int year;
    std::string description;

public:
    AcademicLevel();
    AcademicLevel(const std::string& id, const std::string& name, 
                  int year, const std::string& description);

    // Getters
    std::string getId() const;
    std::string getName() const;
    int getYear() const;
    std::string getDescription() const;

    // Setters
    void setId(const std::string& id);
    void setName(const std::string& name);
    void setYear(int year);
    void setDescription(const std::string& description);

    // Static database operations
    static std::vector<AcademicLevel> getAllLevels(Database& db);
    static AcademicLevel getById(Database& db, const std::string& id);
    static bool exists(Database& db, const std::string& id);

    // Instance database operations
    bool save(Database& db);
    bool update(Database& db);
    bool remove(Database& db);

    // Utility methods
    std::string toJson() const;
    bool isEmpty() const;
};

#endif // ACADEMICLEVEL_H
