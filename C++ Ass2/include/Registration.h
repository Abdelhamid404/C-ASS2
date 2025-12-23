/*
 * ============================================
 * REGISTRATION.H - Registration Class Definition
 * ============================================
 */

#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <string>
#include <vector>
#include "Database.h"

class Registration {
private:
    std::string id;          // Auto-incremented registration ID
    std::string studentId;   // Student ID
    std::string courseId;    // Course ID
    std::string semesterId;  // Semester ID
    std::string status;      // registered/dropped/completed
    std::string createdAt;   // timestamp

public:
    Registration();
    Registration(const std::string& id, const std::string& studentId,
                 const std::string& courseId, const std::string& semesterId);

    // Getters
    std::string getId() const;
    std::string getStudentId() const;
    std::string getCourseId() const;
    std::string getSemesterId() const;
    std::string getStatus() const;
    std::string getCreatedAt() const;

    // Setters
    void setId(const std::string& id);
    void setStudentId(const std::string& studentId);
    void setCourseId(const std::string& courseId);
    void setSemesterId(const std::string& semesterId);
    void setStatus(const std::string& status);

    // Static database operations
    static std::vector<Registration> getAllRegistrations(Database& db);
    static Registration getById(Database& db, const std::string& id);
    static std::vector<Registration> getByStudentId(Database& db, const std::string& studentId);
    static std::vector<Registration> getByCourseId(Database& db, const std::string& courseId,
                                                   const std::string& semesterId);
    static std::vector<Registration> getBySemester(Database& db, const std::string& semesterId);
    static bool exists(Database& db, const std::string& id);
    static bool isStudentRegistered(Database& db, const std::string& studentId,
                                    const std::string& courseId,
                                    const std::string& semesterId);

    // Instance database operations
    bool save(Database& db, const std::string& registeredBy);
    bool update(Database& db);
    bool remove(Database& db);
    bool drop(Database& db);
    bool complete(Database& db);

    // Utility
    std::string toJson() const;
    bool isEmpty() const;
};

#endif // REGISTRATION_H
