/*
 * ============================================
 * STUDENT.H - Student Class Definition
 * ============================================
 * This class represents a student in the system.
 * It contains all student information and CRUD operations.
 */

#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>
#include "Database.h"

/*
 * Student class - Represents a student entity
 * 
 * Contains:
 * - Student personal information (name, email, phone, etc.)
 * - Academic information (level, department)
 * - Database operations (CRUD)
 */
class Student {
private:
    // Student attributes
    std::string id;              // Unique student ID (e.g., "STU001")
    std::string userId;          // Linked users.id (RBAC)
    std::string firstName;       // First name
    std::string lastName;        // Last name
    std::string dateOfBirth;     // Date of birth (YYYY-MM-DD format)
    std::string gender;          // "Male" or "Female"
    std::string email;           // Email address
    std::string phone;           // Phone number
    std::string registrationDate;// When student registered
    std::string academicLevelId; // Foreign key to academic_levels
    std::string departmentId;    // Foreign key to departments
    std::string passwordHash;    // Password for login

public:
    // ========================================
    // CONSTRUCTORS
    // ========================================
    
    // Default constructor - creates empty student
    Student();
    
    // Parameterized constructor - creates student with data
    Student(const std::string& id, const std::string& firstName, 
            const std::string& lastName, const std::string& dateOfBirth,
            const std::string& gender, const std::string& email,
            const std::string& phone, const std::string& academicLevelId,
            const std::string& departmentId);

    // ========================================
    // GETTERS - Get attribute values
    // ========================================
    std::string getId() const;
    std::string getUserId() const;
    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getFullName() const;  // Returns "FirstName LastName"
    std::string getDateOfBirth() const;
    std::string getGender() const;
    std::string getEmail() const;
    std::string getPhone() const;
    std::string getRegistrationDate() const;
    std::string getAcademicLevelId() const;
    std::string getDepartmentId() const;

    // ========================================
    // SETTERS - Set attribute values
    // ========================================
    void setId(const std::string& id);
    void setUserId(const std::string& userId);
    void setFirstName(const std::string& firstName);
    void setLastName(const std::string& lastName);
    void setDateOfBirth(const std::string& dateOfBirth);
    void setGender(const std::string& gender);
    void setEmail(const std::string& email);
    void setPhone(const std::string& phone);
    void setAcademicLevelId(const std::string& academicLevelId);
    void setDepartmentId(const std::string& departmentId);
    void setPassword(const std::string& password);

    // ========================================
    // STATIC DATABASE OPERATIONS
    // ========================================
    // These are static because they don't need an instance

    /*
     * getAllStudents - Gets all students from database
     * 
     * Parameters:
     *   db - Reference to Database object
     * 
     * Returns:
     *   Vector of Student objects
     */
    static std::vector<Student> getAllStudents(Database& db);

    /*
     * getById - Gets a single student by ID
     * 
     * Parameters:
     *   db - Reference to Database object
     *   id - Student ID to search for
     * 
     * Returns:
     *   Student object (empty if not found)
     */
    static Student getById(Database& db, const std::string& id);

    /*
     * exists - Checks if a student ID already exists
     * 
     * Parameters:
     *   db - Reference to Database object
     *   id - Student ID to check
     * 
     * Returns:
     *   true if ID exists, false otherwise
     */
    static bool exists(Database& db, const std::string& id);

    /*
     * getByDepartment - Gets all students in a department
     */
    static std::vector<Student> getByDepartment(Database& db, const std::string& departmentId);

    /*
     * getByAcademicLevel - Gets all students at an academic level
     */
    static std::vector<Student> getByAcademicLevel(Database& db, const std::string& levelId);

    /*
     * validateLogin - Validates student login credentials
     * 
     * Parameters:
     *   db - Reference to Database object
     *   id - Student ID
     *   password - Password to check
     * 
     * Returns:
     *   Student object if valid, empty Student if invalid
     */
    static Student validateLogin(Database& db, const std::string& id, const std::string& password);

    /*
     * generateNextId - Generates the next student ID based on year
     * 
     * Format: YYYY + increment (e.g., 20241, 20242, 20243...)
     * 
     * Parameters:
     *   db - Reference to Database object
     * 
     * Returns:
     *   Next available student ID string
     */
    static std::string generateNextId(Database& db);

    // ========================================
    // INSTANCE DATABASE OPERATIONS
    // ========================================
    // These operate on the current instance

    /*
     * save - Inserts this student into the database
     * 
     * Returns:
     *   true if successful, false if ID already exists
     */
    bool save(Database& db);

    /*
     * update - Updates this student in the database
     * 
     * Returns:
     *   true if successful, false otherwise
     */
    bool update(Database& db);

    /*
     * remove - Deletes this student from the database
     * 
     * Returns:
     *   true if successful, false otherwise
     */
    bool remove(Database& db);

    // ========================================
    // UTILITY METHODS
    // ========================================
    
    /*
     * toJson - Converts student to JSON string
     * 
     * Used for sending data to the web frontend.
     */
    std::string toJson() const;

    /*
     * isEmpty - Checks if student is empty (not loaded)
     */
    bool isEmpty() const;
};

#endif // STUDENT_H
