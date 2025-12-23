/*
 * ============================================
 * ATTENDANCE.H - Attendance Class Definition
 * ============================================
 */

#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <string>
#include <vector>
#include "Database.h"

class Attendance {
private:
    std::string id;
    std::string sessionId;
    std::string studentId;
    std::string status;
    std::string remarks;
    std::string recordedBy;
    std::string recordedAt;

public:
    Attendance();
    Attendance(const std::string& id, const std::string& sessionId,
               const std::string& studentId, const std::string& status);

    // Getters
    std::string getId() const;
    std::string getSessionId() const;
    std::string getStudentId() const;
    std::string getStatus() const;
    std::string getRemarks() const;
    std::string getRecordedBy() const;
    std::string getRecordedAt() const;

    // Setters
    void setId(const std::string& id);
    void setSessionId(const std::string& sessionId);
    void setStudentId(const std::string& studentId);
    void setStatus(const std::string& status);
    void setRemarks(const std::string& remarks);
    void setRecordedBy(const std::string& recordedBy);

    // Static database operations
    static std::vector<Attendance> getAllAttendance(Database& db);
    static Attendance getById(Database& db, const std::string& id);
    static std::vector<Attendance> getBySessionId(Database& db, const std::string& sessionId);
    static std::vector<Attendance> getByStudentId(Database& db, const std::string& studentId);
    static bool exists(Database& db, const std::string& sessionId, const std::string& studentId);

    // Instance database operations
    bool save(Database& db);
    bool update(Database& db);
    bool remove(Database& db);

    // Utility
    std::string toJson() const;
    bool isEmpty() const;
};

#endif // ATTENDANCE_H
