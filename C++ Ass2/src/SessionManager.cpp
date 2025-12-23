/*
 * ============================================
 * SESSIONMANAGER.CPP - Implementation
 * ============================================
 */

#include "../include/SessionManager.h"
#include "../include/JsonHelper.h"
#include <iostream>

using namespace std;

// Static member initialization
UserSession SessionManager::currentSession;

/*
 * login - Authenticates user and creates session
 */
bool SessionManager::login(Database& db, const string& username, const string& password) {
    try {
        // Query user with role info
        auto stmt = db.prepareStatement(
            "SELECT u.id, u.username, u.full_name, u.role_id, r.name_en as role_name "
            "FROM users u "
            "JOIN roles r ON u.role_id = r.id "
            "WHERE u.username = ? AND u.password_hash = ? AND u.is_active = TRUE"
        );
        stmt->setString(1, username);
        stmt->setString(2, password);
        auto result = stmt->executeQuery();
        
        if (result->next()) {
            currentSession.userId = result->getString("id");
            currentSession.username = result->getString("username");
            currentSession.fullName = result->getString("full_name");
            currentSession.roleId = result->getString("role_id");
            currentSession.roleName = result->getString("role_name");
            currentSession.isActive = true;
            
            // Load permissions
            currentSession.permissions = loadPermissions(db, currentSession.roleId, currentSession.userId);
            
            // Get linked ID (professor or student)
            if (currentSession.isProfessor()) {
                auto profStmt = db.prepareStatement(
                    "SELECT id FROM professors WHERE user_id = ?"
                );
                profStmt->setString(1, currentSession.userId);
                auto profResult = profStmt->executeQuery();
                if (profResult->next()) {
                    currentSession.linkedId = profResult->getString("id");
                }
            } else if (currentSession.isStudent()) {
                auto stuStmt = db.prepareStatement(
                    "SELECT id FROM students WHERE user_id = ?"
                );
                stuStmt->setString(1, currentSession.userId);
                auto stuResult = stuStmt->executeQuery();
                if (stuResult->next()) {
                    currentSession.linkedId = stuResult->getString("id");
                }
            }
            
            // Update last login
            auto updateStmt = db.prepareStatement(
                "UPDATE users SET last_login = CURRENT_TIMESTAMP WHERE id = ?"
            );
            updateStmt->setString(1, currentSession.userId);
            updateStmt->executeUpdate();
            
            cout << "[Session] User logged in: " << currentSession.fullName 
                 << " (Role: " << currentSession.roleName << ")" << endl;
            
            return true;
        }
        
        return false;
        
    } catch (const exception& e) {
        cerr << "[Session] Login error: " << e.what() << endl;
        return false;
    }
}

/*
 * logout - Clears current session
 */
void SessionManager::logout() {
    cout << "[Session] User logged out: " << currentSession.username << endl;
    currentSession = UserSession();
}

/*
 * getSession - Returns current session
 */
const UserSession& SessionManager::getSession() {
    return currentSession;
}

/*
 * isLoggedIn - Checks if a user is logged in
 */
bool SessionManager::isLoggedIn() {
    return currentSession.isActive;
}

/*
 * hasPermission - Checks if current user has a specific permission
 */
bool SessionManager::hasPermission(const string& permission) {
    if (currentSession.isSuperAdmin()) {
        return true;
    }
    return currentSession.hasPermission(permission);
}

/*
 * canViewAllStudents - Can user see all students?
 */
bool SessionManager::canViewAllStudents() {
    return hasPermission(Permissions::STU_VIEW_ALL);
}

/*
 * canEditStudents - Can user add/edit students?
 */
bool SessionManager::canEditStudents() {
    return hasPermission(Permissions::STU_CREATE) || hasPermission(Permissions::STU_EDIT);
}

/*
 * canViewAllGrades - Can user see all grades?
 */
bool SessionManager::canViewAllGrades() {
    return hasPermission(Permissions::GRD_VIEW_ALL);
}

/*
 * canEnterGrades - Can user enter/modify grades?
 */
bool SessionManager::canEnterGrades() {
    return hasPermission(Permissions::GRD_ENTER);
}

/*
 * canViewAssignedCourses - Can user see assigned courses?
 */
bool SessionManager::canViewAssignedCourses() {
    return hasPermission(Permissions::GRD_VIEW_ASSIGNED);
}

/*
 * canRecordAttendance - Can user record attendance?
 */
bool SessionManager::canRecordAttendance() {
    return hasPermission(Permissions::ATT_RECORD);
}

/*
 * canAccessCourse - Checks if professor is assigned to a course
 */
bool SessionManager::canAccessCourse(Database& db, const string& courseId,
                                     const string& semesterId) {
    // Admins can access all courses
    if (currentSession.isAdmin()) {
        return true;
    }
    
    // Professor must be assigned to the course
    if (currentSession.isProfessor() && !currentSession.linkedId.empty()) {
        try {
            auto stmt = db.prepareStatement(
                "SELECT COUNT(*) as count FROM course_assignments "
            "WHERE professor_id = ? AND course_id = ? AND semester_id = ?"
        );
        stmt->setString(1, currentSession.linkedId);
        stmt->setString(2, courseId);
        stmt->setString(3, semesterId);
            
            auto result = stmt->executeQuery();
            if (result->next() && result->getInt("count") > 0) {
                return true;
            }
        } catch (const exception& e) {
            cerr << "[Session] Error checking course access: " << e.what() << endl;
        }
    }
    
    return false;
}

/*
 * getAssignedCourseIds - Gets list of course IDs assigned to the professor
 */
vector<string> SessionManager::getAssignedCourseIds(Database& db, const string& semesterId) {
    vector<string> courseIds;
    
    if (!currentSession.isProfessor() || currentSession.linkedId.empty()) {
        return courseIds;
    }
    
    try {
        string query =
            "SELECT DISTINCT course_id FROM course_assignments "
            "WHERE professor_id = ?";
        if (!semesterId.empty()) {
            query += " AND semester_id = ?";
        }
        auto stmt = db.prepareStatement(query);
        stmt->setString(1, currentSession.linkedId);
        if (!semesterId.empty()) {
            stmt->setString(2, semesterId);
        }
        
        auto result = stmt->executeQuery();
        while (result->next()) {
            courseIds.push_back(result->getString("course_id"));
        }
    } catch (const exception& e) {
        cerr << "[Session] Error getting assigned courses: " << e.what() << endl;
    }
    
    return courseIds;
}

/*
 * isOwnStudentId - Checks if the student ID belongs to the logged-in student
 */
bool SessionManager::isOwnStudentId(const string& studentId) {
    return currentSession.isStudent() && currentSession.linkedId == studentId;
}

/*
 * loadPermissions - Loads all permissions for a role
 */
set<string> SessionManager::loadPermissions(Database& db, const string& roleId, const string& userId) {
    set<string> permissions;
    
    try {
        auto stmt = db.prepareStatement(
            "SELECT p.name FROM permissions p "
            "JOIN role_permissions rp ON p.id = rp.permission_id "
            "WHERE rp.role_id = ?"
        );
        stmt->setString(1, roleId);
        
        auto result = stmt->executeQuery();
        while (result->next()) {
            permissions.insert(result->getString("name"));
        }

        // Note: user_permissions table not in schema_v2, skipping per-user overrides
        
        cout << "[Session] Loaded " << permissions.size() << " permissions for role: " << roleId << endl;
        
    } catch (const exception& e) {
        cerr << "[Session] Error loading permissions: " << e.what() << endl;
    }
    
    return permissions;
}

/*
 * logAction - Records an action in the audit log
 */
void SessionManager::logAction(Database& db, const string& action, 
                              const string& tableName, const string& recordId,
                              const string& details) {
    try {
        // audit_log.details is JSON in schema_v3
        string payload = "{}";
        if (!details.empty()) {
            vector<pair<string, string>> obj = {
                {"details", JsonHelper::makeString(details)}
            };
            payload = JsonHelper::makeObject(obj);
        }

        auto stmt = db.prepareStatement(
            "INSERT INTO audit_log (user_id, action, table_name, record_id, details) "
            "VALUES (?, ?, ?, ?, ?)"
        );
        stmt->setString(1, currentSession.userId);
        stmt->setString(2, action);
        stmt->setString(3, tableName);
        stmt->setString(4, recordId);
        stmt->setString(5, payload);
        stmt->executeUpdate();
        
    } catch (...) {
        // Silently ignore audit logging failures (table may not exist)
    }
}
