/*
 * ============================================
 * ATTENDANCE.CPP - Attendance Class Implementation
 * ============================================
 */

#include "../include/Attendance.h"
#include <sstream>

using namespace std;

// ========================================
// CONSTRUCTORS
// ========================================

Attendance::Attendance()
    : id(""), sessionId(""), studentId(""), status("present"),
      remarks(""), recordedBy(""), recordedAt("") {
}

Attendance::Attendance(const string& id, const string& sessionId,
                       const string& studentId, const string& status)
    : id(id), sessionId(sessionId), studentId(studentId), status(status),
      remarks(""), recordedBy(""), recordedAt("") {
}

// ========================================
// GETTERS
// ========================================

string Attendance::getId() const { return id; }
string Attendance::getSessionId() const { return sessionId; }
string Attendance::getStudentId() const { return studentId; }
string Attendance::getStatus() const { return status; }
string Attendance::getRemarks() const { return remarks; }
string Attendance::getRecordedBy() const { return recordedBy; }
string Attendance::getRecordedAt() const { return recordedAt; }

// ========================================
// SETTERS
// ========================================

void Attendance::setId(const string& id) { this->id = id; }
void Attendance::setSessionId(const string& sessionId) { this->sessionId = sessionId; }
void Attendance::setStudentId(const string& studentId) { this->studentId = studentId; }
void Attendance::setStatus(const string& status) { this->status = status; }
void Attendance::setRemarks(const string& remarks) { this->remarks = remarks; }
void Attendance::setRecordedBy(const string& recordedBy) { this->recordedBy = recordedBy; }

// ========================================
// STATIC DATABASE OPERATIONS
// ========================================

vector<Attendance> Attendance::getAllAttendance(Database& db) {
    vector<Attendance> attendanceList;

    try {
        auto result = db.executeQuery(
            "SELECT id, session_id, student_id, status, remarks, recorded_by, recorded_at "
            "FROM attendance ORDER BY recorded_at DESC"
        );

        while (result->next()) {
            Attendance att;
            att.id = result->getString("id");
            att.sessionId = result->getString("session_id");
            att.studentId = result->getString("student_id");
            att.status = result->getString("status");
            att.remarks = result->getString("remarks");
            att.recordedBy = result->getString("recorded_by");
            att.recordedAt = result->getString("recorded_at");

            attendanceList.push_back(att);
        }

    } catch (exception& e) {
        cerr << "[Attendance] Error getting all attendance: " << e.what() << endl;
    }

    return attendanceList;
}

Attendance Attendance::getById(Database& db, const string& id) {
    Attendance att;

    try {
        auto stmt = db.prepareStatement(
            "SELECT id, session_id, student_id, status, remarks, recorded_by, recorded_at "
            "FROM attendance WHERE id = ?"
        );
        stmt->setString(1, id);

        auto result = stmt->executeQuery();

        if (result->next()) {
            att.id = result->getString("id");
            att.sessionId = result->getString("session_id");
            att.studentId = result->getString("student_id");
            att.status = result->getString("status");
            att.remarks = result->getString("remarks");
            att.recordedBy = result->getString("recorded_by");
            att.recordedAt = result->getString("recorded_at");
        }

    } catch (exception& e) {
        cerr << "[Attendance] Error getting attendance by ID: " << e.what() << endl;
    }

    return att;
}

vector<Attendance> Attendance::getBySessionId(Database& db, const string& sessionId) {
    vector<Attendance> attendanceList;

    try {
        auto stmt = db.prepareStatement(
            "SELECT id, session_id, student_id, status, remarks, recorded_by, recorded_at "
            "FROM attendance WHERE session_id = ? ORDER BY student_id"
        );
        stmt->setString(1, sessionId);

        auto result = stmt->executeQuery();

        while (result->next()) {
            Attendance att;
            att.id = result->getString("id");
            att.sessionId = result->getString("session_id");
            att.studentId = result->getString("student_id");
            att.status = result->getString("status");
            att.remarks = result->getString("remarks");
            att.recordedBy = result->getString("recorded_by");
            att.recordedAt = result->getString("recorded_at");

            attendanceList.push_back(att);
        }

    } catch (exception& e) {
        cerr << "[Attendance] Error getting attendance by session: " << e.what() << endl;
    }

    return attendanceList;
}

vector<Attendance> Attendance::getByStudentId(Database& db, const string& studentId) {
    vector<Attendance> attendanceList;

    try {
        auto stmt = db.prepareStatement(
            "SELECT id, session_id, student_id, status, remarks, recorded_by, recorded_at "
            "FROM attendance WHERE student_id = ? ORDER BY recorded_at DESC"
        );
        stmt->setString(1, studentId);

        auto result = stmt->executeQuery();

        while (result->next()) {
            Attendance att;
            att.id = result->getString("id");
            att.sessionId = result->getString("session_id");
            att.studentId = result->getString("student_id");
            att.status = result->getString("status");
            att.remarks = result->getString("remarks");
            att.recordedBy = result->getString("recorded_by");
            att.recordedAt = result->getString("recorded_at");

            attendanceList.push_back(att);
        }

    } catch (exception& e) {
        cerr << "[Attendance] Error getting attendance by student: " << e.what() << endl;
    }

    return attendanceList;
}

bool Attendance::exists(Database& db, const string& sessionId, const string& studentId) {
    try {
        auto stmt = db.prepareStatement(
            "SELECT COUNT(*) as count FROM attendance "
            "WHERE session_id = ? AND student_id = ?"
        );
        stmt->setString(1, sessionId);
        stmt->setString(2, studentId);

        auto result = stmt->executeQuery();

        if (result->next()) {
            return result->getInt("count") > 0;
        }

    } catch (exception& e) {
        cerr << "[Attendance] Error checking attendance exists: " << e.what() << endl;
    }

    return false;
}

// ========================================
// INSTANCE DATABASE OPERATIONS
// ========================================

bool Attendance::save(Database& db) {
    try {
        auto stmt = db.prepareStatement(
            "INSERT INTO attendance (session_id, student_id, status, remarks, recorded_by) "
            "VALUES (?, ?, ?, ?, ?)"
        );

        stmt->setString(1, sessionId);
        stmt->setString(2, studentId);
        stmt->setString(3, status);
        if (remarks.empty()) stmt->setNull(4); else stmt->setString(4, remarks);
        if (recordedBy.empty()) stmt->setNull(5); else stmt->setString(5, recordedBy);

        stmt->executeUpdate();
        id = to_string(db.getLastInsertId());

        return true;

    } catch (exception& e) {
        cerr << "[Attendance] Error saving attendance: " << e.what() << endl;
        return false;
    }
}

bool Attendance::update(Database& db) {
    try {
        auto stmt = db.prepareStatement(
            "UPDATE attendance SET status = ?, remarks = ?, recorded_by = ? WHERE id = ?"
        );
        stmt->setString(1, status);
        if (remarks.empty()) stmt->setNull(2); else stmt->setString(2, remarks);
        if (recordedBy.empty()) stmt->setNull(3); else stmt->setString(3, recordedBy);
        stmt->setString(4, id);

        int rowsAffected = stmt->executeUpdate();
        return rowsAffected > 0;

    } catch (exception& e) {
        cerr << "[Attendance] Error updating attendance: " << e.what() << endl;
        return false;
    }
}

bool Attendance::remove(Database& db) {
    try {
        auto stmt = db.prepareStatement("DELETE FROM attendance WHERE id = ?");
        stmt->setString(1, id);

        int rowsAffected = stmt->executeUpdate();
        return rowsAffected > 0;

    } catch (exception& e) {
        cerr << "[Attendance] Error deleting attendance: " << e.what() << endl;
        return false;
    }
}

// ========================================
// UTILITY METHODS
// ========================================

string Attendance::toJson() const {
    stringstream ss;
    ss << "{";
    ss << "\"id\":\"" << id << "\",";
    ss << "\"sessionId\":\"" << sessionId << "\",";
    ss << "\"studentId\":\"" << studentId << "\",";
    ss << "\"status\":\"" << status << "\",";
    ss << "\"remarks\":\"" << remarks << "\",";
    ss << "\"recordedBy\":\"" << recordedBy << "\",";
    ss << "\"recordedAt\":\"" << recordedAt << "\"";
    ss << "}";
    return ss.str();
}

bool Attendance::isEmpty() const {
    return id.empty();
}
