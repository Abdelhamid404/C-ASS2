/*
 * ============================================
 * PROFESSOR.H - Professor Class Definition
 * ============================================
 * This class represents a professor in the system.
 */

#ifndef PROFESSOR_H
#define PROFESSOR_H

#include <string>
#include <vector>
#include "Database.h"

// NOTE: Do NOT use "using namespace std;" in header files as it causes
// conflicts with Windows SDK types like "byte" when used with C++17.

class Professor {
private:
    std::string id;              // Unique professor ID (e.g., "PROF001")
    std::string userId;          // Linked users.id (RBAC)
    std::string firstName;       // First name
    std::string lastName;        // Last name
    std::string email;           // Email address
    std::string phone;           // Phone number
    std::string departmentId;    // Foreign key to departments
    std::string hireDate;        // Date when professor was hired
    std::string specialization;  // Area of specialization
    std::string passwordHash;    // Password for login

public:
    // Constructors
    Professor();
    Professor(const std::string& id, const std::string& firstName, 
              const std::string& lastName, const std::string& email,
              const std::string& phone, const std::string& departmentId,
              const std::string& specialization);

    // Getters
    std::string getId() const;
    std::string getUserId() const;
    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getFullName() const;
    std::string getEmail() const;
    std::string getPhone() const;
    std::string getDepartmentId() const;
    std::string getHireDate() const;
    std::string getSpecialization() const;

    // Setters
    void setId(const std::string& id);
    void setUserId(const std::string& userId);
    void setFirstName(const std::string& firstName);
    void setLastName(const std::string& lastName);
    void setEmail(const std::string& email);
    void setPhone(const std::string& phone);
    void setDepartmentId(const std::string& departmentId);
    void setSpecialization(const std::string& specialization);
    void setPassword(const std::string& password);

    // Static database operations
    static std::vector<Professor> getAllProfessors(Database& db);
    static Professor getById(Database& db, const std::string& id);
    static bool exists(Database& db, const std::string& id);
    static std::vector<Professor> getByDepartment(Database& db, const std::string& departmentId);
    static Professor validateLogin(Database& db, const std::string& id, const std::string& password);

    // Instance database operations
    bool save(Database& db);
    bool update(Database& db);
    bool remove(Database& db);

    // Utility methods
    std::string toJson() const;
    bool isEmpty() const;
};

#endif // PROFESSOR_H
