/*
 * ============================================
 * LECTUREHALL.H - Lecture Hall Class Definition
 * ============================================
 */

#ifndef LECTUREHALL_H
#define LECTUREHALL_H

#include <string>
#include <vector>
#include "Database.h"

// NOTE: Do NOT use "using namespace std;" in header files as it causes
// conflicts with Windows SDK types like "byte" when used with C++17.

class LectureHall {
private:
    std::string id;
    std::string name;
    std::string building;
    int floor;
    int maxCapacity;
    bool hasProjector;
    bool hasAc;

public:
    LectureHall();
    LectureHall(const std::string& id, const std::string& name, 
                const std::string& building, int floor, int maxCapacity,
                bool hasProjector, bool hasAc);

    // Getters
    std::string getId() const;
    std::string getName() const;
    std::string getBuilding() const;
    int getFloor() const;
    int getMaxCapacity() const;
    bool getHasProjector() const;
    bool getHasAc() const;

    // Setters
    void setId(const std::string& id);
    void setName(const std::string& name);
    void setBuilding(const std::string& building);
    void setFloor(int floor);
    void setMaxCapacity(int maxCapacity);
    void setHasProjector(bool hasProjector);
    void setHasAc(bool hasAc);

    // Static database operations
    static std::vector<LectureHall> getAllHalls(Database& db);
    static LectureHall getById(Database& db, const std::string& id);
    static bool exists(Database& db, const std::string& id);

    // Instance database operations
    bool save(Database& db);
    bool update(Database& db);
    bool remove(Database& db);

    // Utility methods
    std::string toJson() const;
    bool isEmpty() const;
};

#endif // LECTUREHALL_H
