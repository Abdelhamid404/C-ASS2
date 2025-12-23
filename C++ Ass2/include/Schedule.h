/*
 * ============================================
 * SCHEDULE.H - Schedule Class Definition
 * ============================================
 */

#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <string>
#include <vector>
#include "Database.h"

// NOTE: Do NOT use "using namespace std;" in header files as it causes
// conflicts with Windows SDK types like "byte" when used with C++17.

class Schedule {
private:
    std::string id;
    std::string dayOfWeek;  // Sunday, Monday, Tuesday, etc.
    std::string startTime;
    std::string endTime;

public:
    Schedule();
    Schedule(const std::string& id, const std::string& dayOfWeek,
             const std::string& startTime, const std::string& endTime);

    // Getters
    std::string getId() const;
    std::string getDayOfWeek() const;
    std::string getStartTime() const;
    std::string getEndTime() const;

    // Setters
    void setId(const std::string& id);
    void setDayOfWeek(const std::string& dayOfWeek);
    void setStartTime(const std::string& startTime);
    void setEndTime(const std::string& endTime);

    // Static database operations
    static std::vector<Schedule> getAllSchedules(Database& db);
    static Schedule getById(Database& db, const std::string& id);
    static std::vector<Schedule> getByDay(Database& db, const std::string& dayOfWeek);
    static bool exists(Database& db, const std::string& id);

    // Instance database operations
    bool save(Database& db);
    bool update(Database& db);
    bool remove(Database& db);

    // Utility methods
    std::string toJson() const;
    bool isEmpty() const;
};

#endif // SCHEDULE_H
