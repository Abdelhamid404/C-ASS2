/*
 * ============================================
 * LABORATORY.H - Laboratory Class Definition
 * ============================================
 */

#ifndef LABORATORY_H
#define LABORATORY_H

#include <string>
#include <vector>
#include "Database.h"

// NOTE: Do NOT use "using namespace std;" in header files as it causes
// conflicts with Windows SDK types like "byte" when used with C++17.

class Laboratory {
private:
    std::string id;
    std::string name;
    std::string building;
    int floor;
    int maxCapacity;
    std::string labType;  // Computer, Physics, Chemistry, Electronics, Other
    int equipmentCount;

public:
    Laboratory();
    Laboratory(const std::string& id, const std::string& name, 
               const std::string& building, int floor, int maxCapacity,
               const std::string& labType, int equipmentCount);

    // Getters
    std::string getId() const;
    std::string getName() const;
    std::string getBuilding() const;
    int getFloor() const;
    int getMaxCapacity() const;
    std::string getLabType() const;
    int getEquipmentCount() const;

    // Setters
    void setId(const std::string& id);
    void setName(const std::string& name);
    void setBuilding(const std::string& building);
    void setFloor(int floor);
    void setMaxCapacity(int maxCapacity);
    void setLabType(const std::string& labType);
    void setEquipmentCount(int equipmentCount);

    // Static database operations
    static std::vector<Laboratory> getAllLabs(Database& db);
    static Laboratory getById(Database& db, const std::string& id);
    static bool exists(Database& db, const std::string& id);

    // Instance database operations
    bool save(Database& db);
    bool update(Database& db);
    bool remove(Database& db);

    // Utility methods
    std::string toJson() const;
    bool isEmpty() const;
};

#endif // LABORATORY_H
