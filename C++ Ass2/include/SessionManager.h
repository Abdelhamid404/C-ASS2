/*
 * ============================================
 * SESSIONMANAGER.H - Role-Based Access Control
 * ============================================
 * Manages user sessions and permission checks
 */

#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include "Database.h"

// User session data
struct UserSession {
    std::string userId;
    std::string username;
    std::string fullName;
    std::string roleId;
    std::string roleName;
    std::string linkedId;      // Professor ID or Student ID
    std::set<std::string> permissions;
    bool isActive;
    
    UserSession() : isActive(false) {}
    
    bool hasPermission(const std::string& perm) const {
        return permissions.find(perm) != permissions.end();
    }
    
    bool isAdmin() const {
        return roleId == "ROLE_STUDENT_AFFAIRS" || roleId == "ROLE_SUPERADMIN";
    }
    
    bool isProfessor() const {
        return roleId == "ROLE_PROFESSOR";
    }
    
    bool isStudent() const {
        return roleId == "ROLE_STUDENT";
    }
    
    bool isSuperAdmin() const {
        return roleId == "ROLE_SUPERADMIN";
    }
};

// Permission constants
namespace Permissions {
    // System
    const std::string SYS_SETTINGS = "system.settings";
    const std::string SYS_LOGS = "system.logs";
    const std::string USER_MANAGE = "user.manage";
    
    // Students
    const std::string STU_CREATE = "student.create";
    const std::string STU_VIEW_ALL = "student.view";
    const std::string STU_VIEW_OWN = "student.view.own";
    const std::string STU_EDIT = "student.edit";
    const std::string STU_DELETE = "student.delete";

    // Professors
    const std::string PROF_CREATE = "professor.create";
    const std::string PROF_VIEW = "professor.view";
    const std::string PROF_EDIT = "professor.edit";
    const std::string PROF_DELETE = "professor.delete";
    
    // Courses
    const std::string CRS_CREATE = "course.create";
    const std::string CRS_VIEW = "course.view";
    const std::string CRS_EDIT = "course.edit";
    const std::string CRS_DELETE = "course.delete";
    const std::string CRS_ASSIGN = "course.assign";
    
    // Registrations
    const std::string REG_CREATE = "registration.create";
    const std::string REG_VIEW = "registration.view";
    const std::string REG_EDIT = "registration.edit";
    const std::string REG_DELETE = "registration.delete";
    
    // Grades
    const std::string GRD_VIEW_ALL = "grade.view.all";
    const std::string GRD_VIEW_OWN = "grade.view.own";
    const std::string GRD_VIEW_ASSIGNED = "grade.view.assigned";
    const std::string GRD_ENTER = "grade.enter";
    
    // Attendance
    const std::string ATT_VIEW_ALL = "attendance.view.all";
    const std::string ATT_VIEW_OWN = "attendance.view.own";
    const std::string ATT_RECORD = "attendance.record";
    
    // Fees
    const std::string FEE_VIEW = "fee.view";
    const std::string FEE_CREATE = "fee.create";
    const std::string FEE_EDIT = "fee.edit";
    const std::string FEE_DELETE = "fee.delete";
    const std::string PAY_CREATE = "payment.create";
    const std::string PAY_VIEW = "payment.view";
    
    // Password viewing
    const std::string PWD_VIEW_ALL = "password.view.all";
    const std::string PWD_VIEW_STU = "password.view.student";
    
    // Role management
    const std::string ROLE_MANAGE = "role.manage";
}

// Exclusive permissions that only Super Admin can have
namespace ExclusivePermissions {
    inline const std::vector<std::string> SUPER_ADMIN_ONLY = {
        "user.manage",
        "role.manage",
        "password.view.all",
        "system.settings",
        "system.logs",
        "fee.delete"
    };
    
    inline bool isExclusive(const std::string& permission) {
        return std::find(SUPER_ADMIN_ONLY.begin(), SUPER_ADMIN_ONLY.end(), permission) 
               != SUPER_ADMIN_ONLY.end();
    }
}

class SessionManager {
private:
    static UserSession currentSession;
    
public:
    // Login and create session
    static bool login(Database& db, const std::string& username, const std::string& password);
    
    // Logout and clear session
    static void logout();
    
    // Get current session
    static const UserSession& getSession();
    
    // Check if user is logged in
    static bool isLoggedIn();
    
    // Permission checks
    static bool hasPermission(const std::string& permission);
    static bool canViewAllStudents();
    static bool canEditStudents();
    static bool canViewAllGrades();
    static bool canEnterGrades();
    static bool canViewAssignedCourses();
    static bool canRecordAttendance();
    
    // Professor-specific checks
    static bool canAccessCourse(Database& db, const std::string& courseId,
                                const std::string& semesterId);
    static std::vector<std::string> getAssignedCourseIds(Database& db,
                                                         const std::string& semesterId = "");
    
    // Student-specific checks
    static bool isOwnStudentId(const std::string& studentId);
    
    // Load permissions for a role
    static std::set<std::string> loadPermissions(Database& db, const std::string& roleId,
                                                 const std::string& userId);
    
    // Audit logging
    static void logAction(Database& db, const std::string& action, 
                         const std::string& tableName, const std::string& recordId,
                         const std::string& details = "");
};

#endif // SESSION_MANAGER_H
