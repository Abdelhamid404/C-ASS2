/*
 * ============================================
 * STUDENT.CPP - Student Class Implementation
 * ============================================
 * This file contains the implementation of all Student class methods.
 */

#include "../include/Student.h"
#include <sstream>
#include <ctime>
#include <cstdlib>

using namespace std;

// ========================================
// CONSTRUCTORS
// ========================================

// Default constructor - creates empty student
Student::Student() 
    : id(""), userId(""), firstName(""), lastName(""), dateOfBirth(""),
      gender(""), email(""), phone(""), registrationDate(""),
      academicLevelId(""), departmentId(""), passwordHash("student123") {
}

// Parameterized constructor - creates student with data
Student::Student(const string& id, const string& firstName, 
                 const string& lastName, const string& dateOfBirth,
                 const string& gender, const string& email,
                 const string& phone, const string& academicLevelId,
                 const string& departmentId)
    : id(id), userId(""), firstName(firstName), lastName(lastName), 
      dateOfBirth(dateOfBirth), gender(gender), email(email),
      phone(phone), academicLevelId(academicLevelId), 
      departmentId(departmentId), passwordHash("student123") {
}

// ========================================
// GETTERS
// ========================================

string Student::getId() const { return id; }
string Student::getUserId() const { return userId; }
string Student::getFirstName() const { return firstName; }
string Student::getLastName() const { return lastName; }
string Student::getFullName() const { return firstName + " " + lastName; }
string Student::getDateOfBirth() const { return dateOfBirth; }
string Student::getGender() const { return gender; }
string Student::getEmail() const { return email; }
string Student::getPhone() const { return phone; }
string Student::getRegistrationDate() const { return registrationDate; }
string Student::getAcademicLevelId() const { return academicLevelId; }
string Student::getDepartmentId() const { return departmentId; }

// ========================================
// SETTERS
// ========================================

void Student::setId(const string& id) { this->id = id; }
void Student::setUserId(const string& userId) { this->userId = userId; }
void Student::setFirstName(const string& firstName) { this->firstName = firstName; }
void Student::setLastName(const string& lastName) { this->lastName = lastName; }
void Student::setDateOfBirth(const string& dateOfBirth) { this->dateOfBirth = dateOfBirth; }
void Student::setGender(const string& gender) { this->gender = gender; }
void Student::setEmail(const string& email) { this->email = email; }
void Student::setPhone(const string& phone) { this->phone = phone; }
void Student::setAcademicLevelId(const string& academicLevelId) { this->academicLevelId = academicLevelId; }
void Student::setDepartmentId(const string& departmentId) { this->departmentId = departmentId; }
void Student::setPassword(const string& password) { this->passwordHash = password; }

// ========================================
// STATIC DATABASE OPERATIONS
// ========================================

/*
 * getAllStudents - Retrieves all students from database
 */
vector<Student> Student::getAllStudents(Database& db) {
    vector<Student> students;
    
    try {
        // Execute query to get all students
        auto result = db.executeQuery(
            "SELECT s.id, s.user_id, s.first_name, s.last_name, s.date_of_birth, s.gender, "
            "u.email, u.phone, s.enrollment_date, s.academic_level_id, s.department_id "
            "FROM students s "
            "LEFT JOIN users u ON s.user_id = u.id "
            "ORDER BY s.id"
        );
        
        // Loop through results and create Student objects
        while (result->next()) {
            Student student;
            student.id = result->getString("id");
            student.userId = result->getString("user_id");
            student.firstName = result->getString("first_name");
            student.lastName = result->getString("last_name");
            student.dateOfBirth = result->getString("date_of_birth");
            student.gender = result->getString("gender");
            student.email = result->getString("email");
            student.phone = result->getString("phone");
            student.registrationDate = result->getString("enrollment_date");
            student.academicLevelId = result->getString("academic_level_id");
            student.departmentId = result->getString("department_id");
            
            students.push_back(student);
        }
        
    } catch (exception& e) {
        cerr << "[Student] Error getting all students: " << e.what() << endl;
    }
    
    return students;
}

/*
 * getById - Gets a single student by their ID
 */
Student Student::getById(Database& db, const string& id) {
    Student student;
    
    try {
        // Use prepared statement to prevent SQL injection
        auto stmt = db.prepareStatement(
            "SELECT s.id, s.user_id, s.first_name, s.last_name, s.date_of_birth, s.gender, "
            "u.email, u.phone, s.enrollment_date, s.academic_level_id, s.department_id "
            "FROM students s "
            "LEFT JOIN users u ON s.user_id = u.id "
            "WHERE s.id = ?"
        );
        stmt->setString(1, id);
        
        auto result = stmt->executeQuery();
        
        if (result->next()) {
            student.id = result->getString("id");
            student.userId = result->getString("user_id");
            student.firstName = result->getString("first_name");
            student.lastName = result->getString("last_name");
            student.dateOfBirth = result->getString("date_of_birth");
            student.gender = result->getString("gender");
            student.email = result->getString("email");
            student.phone = result->getString("phone");
            student.registrationDate = result->getString("enrollment_date");
            student.academicLevelId = result->getString("academic_level_id");
            student.departmentId = result->getString("department_id");
        }
        
    } catch (exception& e) {
        cerr << "[Student] Error getting student by ID: " << e.what() << endl;
    }
    
    return student;
}

/*
 * exists - Checks if a student ID already exists
 */
bool Student::exists(Database& db, const string& id) {
    try {
        auto stmt = db.prepareStatement("SELECT COUNT(*) as count FROM students WHERE id = ?");
        stmt->setString(1, id);
        
        auto result = stmt->executeQuery();
        
        if (result->next()) {
            return result->getInt("count") > 0;
        }
        
    } catch (exception& e) {
        cerr << "[Student] Error checking if student exists: " << e.what() << endl;
    }
    
    return false;
}

/*
 * getByDepartment - Gets all students in a specific department
 */
vector<Student> Student::getByDepartment(Database& db, const string& departmentId) {
    vector<Student> students;
    
    try {
        auto stmt = db.prepareStatement(
            "SELECT s.id, s.user_id, s.first_name, s.last_name, s.date_of_birth, s.gender, "
            "u.email, u.phone, s.enrollment_date, s.academic_level_id, s.department_id "
            "FROM students s "
            "LEFT JOIN users u ON s.user_id = u.id "
            "WHERE s.department_id = ? ORDER BY s.id"
        );
        stmt->setString(1, departmentId);
        
        auto result = stmt->executeQuery();
        
        while (result->next()) {
            Student student;
            student.id = result->getString("id");
            student.userId = result->getString("user_id");
            student.firstName = result->getString("first_name");
            student.lastName = result->getString("last_name");
            student.dateOfBirth = result->getString("date_of_birth");
            student.gender = result->getString("gender");
            student.email = result->getString("email");
            student.phone = result->getString("phone");
            student.registrationDate = result->getString("enrollment_date");
            student.academicLevelId = result->getString("academic_level_id");
            student.departmentId = result->getString("department_id");
            
            students.push_back(student);
        }
        
    } catch (exception& e) {
        cerr << "[Student] Error getting students by department: " << e.what() << endl;
    }
    
    return students;
}

/*
 * getByAcademicLevel - Gets all students at a specific academic level
 */
vector<Student> Student::getByAcademicLevel(Database& db, const string& levelId) {
    vector<Student> students;
    
    try {
        auto stmt = db.prepareStatement(
            "SELECT s.id, s.user_id, s.first_name, s.last_name, s.date_of_birth, s.gender, "
            "u.email, u.phone, s.enrollment_date, s.academic_level_id, s.department_id "
            "FROM students s "
            "LEFT JOIN users u ON s.user_id = u.id "
            "WHERE s.academic_level_id = ? ORDER BY s.id"
        );
        stmt->setString(1, levelId);
        
        auto result = stmt->executeQuery();
        
        while (result->next()) {
            Student student;
            student.id = result->getString("id");
            student.userId = result->getString("user_id");
            student.firstName = result->getString("first_name");
            student.lastName = result->getString("last_name");
            student.dateOfBirth = result->getString("date_of_birth");
            student.gender = result->getString("gender");
            student.email = result->getString("email");
            student.phone = result->getString("phone");
            student.registrationDate = result->getString("enrollment_date");
            student.academicLevelId = result->getString("academic_level_id");
            student.departmentId = result->getString("department_id");
            
            students.push_back(student);
        }
        
    } catch (exception& e) {
        cerr << "[Student] Error getting students by level: " << e.what() << endl;
    }
    
    return students;
}

/*
 * validateLogin - Validates student login credentials
 */
Student Student::validateLogin(Database& db, const string& id, const string& password) {
    Student student;
    
    try {
        auto stmt = db.prepareStatement(
            "SELECT s.id, s.user_id, s.first_name, s.last_name, s.date_of_birth, s.gender, "
            "u.email, u.phone, s.enrollment_date, s.academic_level_id, s.department_id "
            "FROM users u "
            "JOIN students s ON s.user_id = u.id "
            "WHERE u.username = ? AND u.password_hash = ? AND u.is_active = TRUE AND u.role_id = 'ROLE_STUDENT'"
        );
        stmt->setString(1, id);
        stmt->setString(2, password);
        
        auto result = stmt->executeQuery();
        
        if (result->next()) {
            student.id = result->getString("id");
            student.userId = result->getString("user_id");
            student.firstName = result->getString("first_name");
            student.lastName = result->getString("last_name");
            student.dateOfBirth = result->getString("date_of_birth");
            student.gender = result->getString("gender");
            student.email = result->getString("email");
            student.phone = result->getString("phone");
            student.registrationDate = result->getString("enrollment_date");
            student.academicLevelId = result->getString("academic_level_id");
            student.departmentId = result->getString("department_id");
        }
        
    } catch (exception& e) {
        cerr << "[Student] Error validating login: " << e.what() << endl;
    }
    
    return student;
}

/*
 * generateNextId - Generates the next student ID based on current year
 * 
 * Format: YYYY + increment (e.g., 20241, 20242, 20243...)
 * Example: First student in 2024 = 20241, second = 20242, etc.
 */
string Student::generateNextId(Database& db) {
    auto getCurrentYear = []() -> int {
        time_t now = time(nullptr);
        tm timeInfo{};
#if defined(_WIN32)
        localtime_s(&timeInfo, &now);
#else
        localtime_r(&now, &timeInfo);
#endif
        return 1900 + timeInfo.tm_year;
    };

    try {
        // Get current year
        string yearStr = to_string(getCurrentYear());
        
        // Find the maximum ID that starts with current year
        auto stmt = db.prepareStatement(
            "SELECT MAX(CAST(id AS UNSIGNED)) as max_id FROM students WHERE id LIKE ?"
        );
        stmt->setString(1, yearStr + "%");
        
        auto result = stmt->executeQuery();
        
        int nextNumber = 1;
        if (result->next()) {
            string maxId = result->getString("max_id");
            if (!maxId.empty() && maxId != "0") {
                // Extract the number part after the year
                if (maxId.length() > 4) {
                    int lastNumber = stoi(maxId.substr(4));
                    nextNumber = lastNumber + 1;
                } else {
                    // ID is just the year, so next is 1
                    nextNumber = 1;
                }
            }
        }
        
        // Generate the new ID: YYYY + increment
        string newId = yearStr + to_string(nextNumber);
        
        cout << "[Student] Generated new ID: " << newId << endl;
        return newId;
        
    } catch (exception& e) {
        cerr << "[Student] Error generating ID: " << e.what() << endl;
        
        // Fallback: use year + random number
        return to_string(getCurrentYear()) + to_string(rand() % 1000 + 1);
    }
}

// ========================================
// INSTANCE DATABASE OPERATIONS
// ========================================

/*
 * save - Inserts this student into the database
 */
bool Student::save(Database& db) {
    // First check if ID already exists
    if (exists(db, this->id)) {
        cerr << "[Student] Error: Student ID '" << this->id << "' already exists!" << endl;
        return false;
    }
    
    try {
        db.executeUpdate("START TRANSACTION");

        // Prevent duplicate usernames
        auto userCheck = db.prepareStatement("SELECT COUNT(*) as cnt FROM users WHERE username = ?");
        userCheck->setString(1, id);
        auto userCheckResult = userCheck->executeQuery();
        if (userCheckResult->next() && userCheckResult->getInt("cnt") > 0) {
            db.executeUpdate("ROLLBACK");
            cerr << "[Student] Error: Username '" << id << "' already exists!" << endl;
            return false;
        }

        // Create user (RBAC)
        auto userStmt = db.prepareStatement(
            "INSERT INTO users (username, password_hash, full_name, email, phone, role_id) "
            "VALUES (?, ?, ?, ?, ?, 'ROLE_STUDENT')"
        );
        userStmt->setString(1, id);
        userStmt->setString(2, passwordHash);
        userStmt->setString(3, getFullName());
        if (email.empty()) userStmt->setNull(4); else userStmt->setString(4, email);
        if (phone.empty()) userStmt->setNull(5); else userStmt->setString(5, phone);
        userStmt->executeUpdate();

        userId = to_string(db.getLastInsertId());

        // Create student profile
        auto stmt = db.prepareStatement(
            "INSERT INTO students (id, user_id, first_name, last_name, date_of_birth, "
            "gender, academic_level_id, department_id) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?)"
        );
        stmt->setString(1, id);
        stmt->setString(2, userId);
        stmt->setString(3, firstName);
        stmt->setString(4, lastName);
        if (dateOfBirth.empty()) stmt->setNull(5); else stmt->setString(5, dateOfBirth);
        stmt->setString(6, gender);
        stmt->setString(7, academicLevelId);
        stmt->setString(8, departmentId);
        stmt->executeUpdate();

        db.executeUpdate("COMMIT");
        
        cout << "[Student] Successfully added student: " << id << endl;
        return true;
        
    } catch (exception& e) {
        try { db.executeUpdate("ROLLBACK"); } catch (...) {}
        cerr << "[Student] Error saving student: " << e.what() << endl;
        return false;
    }
}

/*
 * update - Updates this student in the database
 */
bool Student::update(Database& db) {
    try {
        auto userIdStmt = db.prepareStatement("SELECT user_id FROM students WHERE id = ?");
        userIdStmt->setString(1, id);
        auto userIdResult = userIdStmt->executeQuery();
        if (!userIdResult->next()) {
            cerr << "[Student] No student found with ID: " << id << endl;
            return false;
        }
        userId = userIdResult->getString("user_id");

        db.executeUpdate("START TRANSACTION");

        // Update student profile
        auto stmt = db.prepareStatement(
            "UPDATE students SET first_name = ?, last_name = ?, date_of_birth = ?, "
            "gender = ?, academic_level_id = ?, department_id = ? "
            "WHERE id = ?"
        );
        stmt->setString(1, firstName);
        stmt->setString(2, lastName);
        if (dateOfBirth.empty()) stmt->setNull(3); else stmt->setString(3, dateOfBirth);
        stmt->setString(4, gender);
        stmt->setString(5, academicLevelId);
        stmt->setString(6, departmentId);
        stmt->setString(7, id);
        stmt->executeUpdate();

        // Update linked user record (email/phone are stored in users)
        if (!userId.empty()) {
            auto userStmt = db.prepareStatement(
                "UPDATE users SET full_name = ?, email = ?, phone = ? WHERE id = ?"
            );
            userStmt->setString(1, getFullName());
            if (email.empty()) userStmt->setNull(2); else userStmt->setString(2, email);
            if (phone.empty()) userStmt->setNull(3); else userStmt->setString(3, phone);
            userStmt->setString(4, userId);
            userStmt->executeUpdate();
        }

        // MySQL may report 0 affected rows if values are unchanged; treat as success.
        db.executeUpdate("COMMIT");
        cout << "[Student] Successfully updated student: " << id << endl;
        return true;
        
    } catch (exception& e) {
        try { db.executeUpdate("ROLLBACK"); } catch (...) {}
        cerr << "[Student] Error updating student: " << e.what() << endl;
        return false;
    }
}

/*
 * remove - Deletes this student from the database
 */
bool Student::remove(Database& db) {
    try {
        if (userId.empty()) {
            auto userIdStmt = db.prepareStatement("SELECT user_id FROM students WHERE id = ?");
            userIdStmt->setString(1, id);
            auto userIdResult = userIdStmt->executeQuery();
            if (userIdResult->next()) {
                userId = userIdResult->getString("user_id");
            }
        }

        db.executeUpdate("START TRANSACTION");

        auto stmt = db.prepareStatement("DELETE FROM students WHERE id = ?");
        stmt->setString(1, id);
        int rowsAffected = stmt->executeUpdate();

        if (rowsAffected > 0 && !userId.empty()) {
            auto userStmt = db.prepareStatement("DELETE FROM users WHERE id = ?");
            userStmt->setString(1, userId);
            userStmt->executeUpdate();
        }
        
        if (rowsAffected > 0) {
            db.executeUpdate("COMMIT");
            cout << "[Student] Successfully deleted student: " << id << endl;
            return true;
        } else {
            db.executeUpdate("ROLLBACK");
            cerr << "[Student] No student found with ID: " << id << endl;
            return false;
        }
        
    } catch (exception& e) {
        try { db.executeUpdate("ROLLBACK"); } catch (...) {}
        cerr << "[Student] Error deleting student: " << e.what() << endl;
        return false;
    }
}

// ========================================
// UTILITY METHODS
// ========================================

/*
 * toJson - Converts student to JSON string
 */
string Student::toJson() const {
    stringstream ss;
    ss << "{";
    ss << "\"id\":\"" << id << "\",";
    ss << "\"firstName\":\"" << firstName << "\",";
    ss << "\"lastName\":\"" << lastName << "\",";
    ss << "\"fullName\":\"" << getFullName() << "\",";
    ss << "\"dateOfBirth\":\"" << dateOfBirth << "\",";
    ss << "\"gender\":\"" << gender << "\",";
    ss << "\"email\":\"" << email << "\",";
    ss << "\"phone\":\"" << phone << "\",";
    ss << "\"registrationDate\":\"" << registrationDate << "\",";
    ss << "\"academicLevelId\":\"" << academicLevelId << "\",";
    ss << "\"departmentId\":\"" << departmentId << "\"";
    ss << "}";
    return ss.str();
}

/*
 * isEmpty - Checks if student is empty (not loaded)
 */
bool Student::isEmpty() const {
    return id.empty();
}
