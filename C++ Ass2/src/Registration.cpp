/*
 * ============================================
 * REGISTRATION.CPP - Registration Class Implementation
 * ============================================
 */

#include "../include/Registration.h"
#include <sstream>

using namespace std;

// ========================================
// CONSTRUCTORS
// ========================================

Registration::Registration()
    : id(""), studentId(""), courseId(""), semesterId(""),
      status("registered"), createdAt("") {
}

Registration::Registration(const string& id, const string& studentId,
                           const string& courseId, const string& semesterId)
    : id(id), studentId(studentId), courseId(courseId), semesterId(semesterId),
      status("registered"), createdAt("") {
}

// ========================================
// GETTERS
// ========================================

string Registration::getId() const { return id; }
string Registration::getStudentId() const { return studentId; }
string Registration::getCourseId() const { return courseId; }
string Registration::getSemesterId() const { return semesterId; }
string Registration::getStatus() const { return status; }
string Registration::getCreatedAt() const { return createdAt; }

// ========================================
// SETTERS
// ========================================

void Registration::setId(const string& id) { this->id = id; }
void Registration::setStudentId(const string& studentId) { this->studentId = studentId; }
void Registration::setCourseId(const string& courseId) { this->courseId = courseId; }
void Registration::setSemesterId(const string& semesterId) { this->semesterId = semesterId; }
void Registration::setStatus(const string& status) { this->status = status; }

// ========================================
// STATIC DATABASE OPERATIONS
// ========================================

vector<Registration> Registration::getAllRegistrations(Database& db) {
    vector<Registration> registrations;

    try {
        auto result = db.executeQuery(
            "SELECT id, student_id, course_id, semester_id, status, created_at "
            "FROM registrations ORDER BY id"
        );

        while (result->next()) {
            Registration reg;
            reg.id = result->getString("id");
            reg.studentId = result->getString("student_id");
            reg.courseId = result->getString("course_id");
            reg.semesterId = result->getString("semester_id");
            reg.status = result->getString("status");
            reg.createdAt = result->getString("created_at");

            registrations.push_back(reg);
        }

    } catch (exception& e) {
        cerr << "[Registration] Error getting all registrations: " << e.what() << endl;
    }

    return registrations;
}

Registration Registration::getById(Database& db, const string& id) {
    Registration reg;

    try {
        auto stmt = db.prepareStatement(
            "SELECT id, student_id, course_id, semester_id, status, created_at "
            "FROM registrations WHERE id = ?"
        );
        stmt->setString(1, id);

        auto result = stmt->executeQuery();

        if (result->next()) {
            reg.id = result->getString("id");
            reg.studentId = result->getString("student_id");
            reg.courseId = result->getString("course_id");
            reg.semesterId = result->getString("semester_id");
            reg.status = result->getString("status");
            reg.createdAt = result->getString("created_at");
        }

    } catch (exception& e) {
        cerr << "[Registration] Error getting registration by ID: " << e.what() << endl;
    }

    return reg;
}

vector<Registration> Registration::getByStudentId(Database& db, const string& studentId) {
    vector<Registration> registrations;

    try {
        auto stmt = db.prepareStatement(
            "SELECT id, student_id, course_id, semester_id, status, created_at "
            "FROM registrations WHERE student_id = ? ORDER BY created_at DESC"
        );
        stmt->setString(1, studentId);

        auto result = stmt->executeQuery();

        while (result->next()) {
            Registration reg;
            reg.id = result->getString("id");
            reg.studentId = result->getString("student_id");
            reg.courseId = result->getString("course_id");
            reg.semesterId = result->getString("semester_id");
            reg.status = result->getString("status");
            reg.createdAt = result->getString("created_at");

            registrations.push_back(reg);
        }

    } catch (exception& e) {
        cerr << "[Registration] Error getting registrations by student: " << e.what() << endl;
    }

    return registrations;
}

vector<Registration> Registration::getByCourseId(Database& db, const string& courseId,
                                                const string& semesterId) {
    vector<Registration> registrations;

    try {
        auto stmt = db.prepareStatement(
            "SELECT id, student_id, course_id, semester_id, status, created_at "
            "FROM registrations WHERE course_id = ? AND semester_id = ? ORDER BY student_id"
        );
        stmt->setString(1, courseId);
        stmt->setString(2, semesterId);

        auto result = stmt->executeQuery();

        while (result->next()) {
            Registration reg;
            reg.id = result->getString("id");
            reg.studentId = result->getString("student_id");
            reg.courseId = result->getString("course_id");
            reg.semesterId = result->getString("semester_id");
            reg.status = result->getString("status");
            reg.createdAt = result->getString("created_at");

            registrations.push_back(reg);
        }

    } catch (exception& e) {
        cerr << "[Registration] Error getting registrations by course: " << e.what() << endl;
    }

    return registrations;
}

vector<Registration> Registration::getBySemester(Database& db, const string& semesterId) {
    vector<Registration> registrations;

    try {
        auto stmt = db.prepareStatement(
            "SELECT id, student_id, course_id, semester_id, status, created_at "
            "FROM registrations WHERE semester_id = ? ORDER BY student_id"
        );
        stmt->setString(1, semesterId);

        auto result = stmt->executeQuery();

        while (result->next()) {
            Registration reg;
            reg.id = result->getString("id");
            reg.studentId = result->getString("student_id");
            reg.courseId = result->getString("course_id");
            reg.semesterId = result->getString("semester_id");
            reg.status = result->getString("status");
            reg.createdAt = result->getString("created_at");

            registrations.push_back(reg);
        }

    } catch (exception& e) {
        cerr << "[Registration] Error getting registrations by semester: " << e.what() << endl;
    }

    return registrations;
}

bool Registration::exists(Database& db, const string& id) {
    try {
        auto stmt = db.prepareStatement("SELECT COUNT(*) as count FROM registrations WHERE id = ?");
        stmt->setString(1, id);

        auto result = stmt->executeQuery();

        if (result->next()) {
            return result->getInt("count") > 0;
        }

    } catch (exception& e) {
        cerr << "[Registration] Error checking if registration exists: " << e.what() << endl;
    }

    return false;
}

bool Registration::isStudentRegistered(Database& db, const string& studentId,
                                       const string& courseId,
                                       const string& semesterId) {
    try {
        auto stmt = db.prepareStatement(
            "SELECT COUNT(*) as count FROM registrations "
            "WHERE student_id = ? AND course_id = ? AND semester_id = ?"
        );
        stmt->setString(1, studentId);
        stmt->setString(2, courseId);
        stmt->setString(3, semesterId);

        auto result = stmt->executeQuery();

        if (result->next()) {
            return result->getInt("count") > 0;
        }

    } catch (exception& e) {
        cerr << "[Registration] Error checking registration: " << e.what() << endl;
    }

    return false;
}

// ========================================
// INSTANCE DATABASE OPERATIONS
// ========================================

bool Registration::save(Database& db, const string& registeredBy) {
    if (isStudentRegistered(db, studentId, courseId, semesterId)) {
        cerr << "[Registration] Student already registered for this course!" << endl;
        return false;
    }

    try {
        auto stmt = db.prepareStatement(
            "INSERT INTO registrations (student_id, course_id, semester_id, status, registered_by) "
            "VALUES (?, ?, ?, ?, ?)"
        );

        stmt->setString(1, studentId);
        stmt->setString(2, courseId);
        stmt->setString(3, semesterId);
        stmt->setString(4, status);
        if (registeredBy.empty()) stmt->setNull(5); else stmt->setString(5, registeredBy);

        stmt->executeUpdate();

        id = to_string(db.getLastInsertId());

        // Create empty grade record for this registration
        auto gradeStmt = db.prepareStatement("INSERT INTO grades (registration_id) VALUES (?)");
        gradeStmt->setString(1, id);
        gradeStmt->executeUpdate();

        cout << "[Registration] Successfully added registration: " << id << endl;
        return true;

    } catch (exception& e) {
        cerr << "[Registration] Error saving registration: " << e.what() << endl;
        return false;
    }
}

bool Registration::update(Database& db) {
    try {
        auto stmt = db.prepareStatement(
            "UPDATE registrations SET status = ? WHERE id = ?"
        );

        stmt->setString(1, status);
        stmt->setString(2, id);

        int rowsAffected = stmt->executeUpdate();
        return rowsAffected > 0;

    } catch (exception& e) {
        cerr << "[Registration] Error updating registration: " << e.what() << endl;
        return false;
    }
}

bool Registration::remove(Database& db) {
    try {
        auto stmt = db.prepareStatement("DELETE FROM registrations WHERE id = ?");
        stmt->setString(1, id);

        int rowsAffected = stmt->executeUpdate();
        return rowsAffected > 0;

    } catch (exception& e) {
        cerr << "[Registration] Error deleting registration: " << e.what() << endl;
        return false;
    }
}

bool Registration::drop(Database& db) {
    status = "dropped";
    return update(db);
}

bool Registration::complete(Database& db) {
    status = "completed";
    return update(db);
}

// ========================================
// UTILITY METHODS
// ========================================

string Registration::toJson() const {
    stringstream ss;
    ss << "{";
    ss << "\"id\":\"" << id << "\",";
    ss << "\"studentId\":\"" << studentId << "\",";
    ss << "\"courseId\":\"" << courseId << "\",";
    ss << "\"semesterId\":\"" << semesterId << "\",";
    ss << "\"status\":\"" << status << "\",";
    ss << "\"createdAt\":\"" << createdAt << "\"";
    ss << "}";
    return ss.str();
}

bool Registration::isEmpty() const {
    return id.empty();
}
