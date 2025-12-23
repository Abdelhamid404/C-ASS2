/*
 * ============================================
 * MAIN.CPP - Student Information System v2.0
 * ============================================
 * NCTU - With Role-Based Access Control (RBAC)
 */

#include "include/Database.h"
#include "include/Student.h"
#include "include/Professor.h"
#include "include/Course.h"
#include "include/Grade.h"
#include "include/JsonHelper.h"
#include "include/SessionManager.h"
#include "lib/webview.h"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <filesystem>

using namespace std;

Database db;

static string urlEncodePath(const string& path) {
    string out;
    out.reserve(path.size());
    for (unsigned char c : path) {
        switch (c) {
            case ' ': out += "%20"; break;
            case '#': out += "%23"; break;
            case '%': out += "%25"; break;
            default: out += static_cast<char>(c); break;
        }
    }
    return out;
}

static string toFileUrl(std::filesystem::path path) {
    path = std::filesystem::absolute(path);
    string p = path.generic_string();

    // Windows drive letter needs a leading slash in file URLs: /C:/...
    if (p.size() >= 2 && std::isalpha(static_cast<unsigned char>(p[0])) && p[1] == ':') {
        p = "/" + p;
    }

    return "file://" + urlEncodePath(p);
}

static bool getCurrentSemester(Database& db, string& semesterId, int& semesterNumber) {
    try {
        auto result = db.executeQuery(
            "SELECT id, semester_number FROM semesters WHERE is_current = TRUE LIMIT 1"
        );
        if (result->next()) {
            semesterId = result->getString("id");
            semesterNumber = result->getInt("semester_number");
            return true;
        }
    } catch (const exception& e) {
        cerr << "[Main] Error loading current semester: " << e.what() << endl;
    }
    return false;
}

int main() {
    cout << "========================================" << endl;
    cout << "   Student Information System v2.0     " << endl;
    cout << "   NCTU - With RBAC                    " << endl;
    cout << "========================================" << endl;
    cout << endl;

    if (!db.testConnection()) {
        cerr << "ERROR: Cannot connect to database!" << endl;
        cerr << "Check your MySQL password in Database.h" << endl;
        cerr << "Press Enter to exit..." << endl;
        cin.get();
        return 1;
    }

    webview::webview w(true, nullptr);
    w.set_title("NCTU - Student Information System");
    w.set_size(1280, 800, WEBVIEW_HINT_NONE);

    cout << "[Main] WebView window created" << endl;

    // ========================================
    // AUTHENTICATION
    // ========================================
    w.bind("cpp_login", [](const string& request) -> string {
        try {
            string username = JsonHelper::parseSimpleValue(request, "username");
            string password = JsonHelper::parseSimpleValue(request, "password");

            cout << "[Login] Attempting login: " << username << endl;

            if (SessionManager::login(db, username, password)) {
                const UserSession& session = SessionManager::getSession();
                
                vector<pair<string, string>> response;
                response.push_back({"success", JsonHelper::makeBool(true)});
                response.push_back({"role", JsonHelper::makeString(session.roleId)});
                response.push_back({"roleName", JsonHelper::makeString(session.roleName)});
                response.push_back({"id", JsonHelper::makeString(session.userId)});
                response.push_back({"linkedId", JsonHelper::makeString(session.linkedId)});
                response.push_back({"name", JsonHelper::makeString(session.fullName)});
                
                string permsJson = "[";
                bool first = true;
                for (const auto& perm : session.permissions) {
                    if (!first) permsJson += ",";
                    permsJson += "\"" + perm + "\"";
                    first = false;
                }
                permsJson += "]";
                response.push_back({"permissions", permsJson});
                
                SessionManager::logAction(db, "LOGIN", "users", session.userId, "Successful login");
                return JsonHelper::makeObject(response);
            } else {
                vector<pair<string, string>> response;
                response.push_back({"success", JsonHelper::makeBool(false)});
                response.push_back({"message", JsonHelper::makeString("Invalid credentials")});
                return JsonHelper::makeObject(response);
            }
        } catch (const exception& e) {
            cerr << "[Login] Error: " << e.what() << endl;
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_logout", [](const string& request) -> string {
        SessionManager::logout();
        return JsonHelper::successResponse("Logged out");
    });

    w.bind("cpp_getSession", [](const string& request) -> string {
        if (!SessionManager::isLoggedIn()) {
            return JsonHelper::errorResponse("Not logged in");
        }
        const UserSession& session = SessionManager::getSession();
        vector<pair<string, string>> response;
        response.push_back({"userId", JsonHelper::makeString(session.userId)});
        response.push_back({"username", JsonHelper::makeString(session.username)});
        response.push_back({"fullName", JsonHelper::makeString(session.fullName)});
        response.push_back({"roleId", JsonHelper::makeString(session.roleId)});
        response.push_back({"roleName", JsonHelper::makeString(session.roleName)});
        response.push_back({"linkedId", JsonHelper::makeString(session.linkedId)});
        
        // Add permissions array
        string permsJson = "[";
        bool first = true;
        for (const auto& perm : session.permissions) {
            if (!first) permsJson += ",";
            permsJson += "\"" + perm + "\"";
            first = false;
        }
        permsJson += "]";
        response.push_back({"permissions", permsJson});
        
        return JsonHelper::makeObject(response);
    });

    // ========================================
    // DASHBOARD
    // ========================================
    w.bind("cpp_getDashboardStats", [](const string& request) -> string {
        try {
            vector<pair<string, string>> stats;
            
            if (SessionManager::canViewAllStudents()) {
                auto result = db.executeQuery("SELECT COUNT(*) as count FROM students");
                if (result->next()) stats.push_back({"students", JsonHelper::makeNumber(result->getInt("count"))});
                
                result = db.executeQuery("SELECT COUNT(*) as count FROM professors");
                if (result->next()) stats.push_back({"professors", JsonHelper::makeNumber(result->getInt("count"))});
                
                result = db.executeQuery("SELECT COUNT(*) as count FROM courses");
                if (result->next()) stats.push_back({"courses", JsonHelper::makeNumber(result->getInt("count"))});
                
                result = db.executeQuery("SELECT COUNT(*) as count FROM departments");
                if (result->next()) stats.push_back({"departments", JsonHelper::makeNumber(result->getInt("count"))});
            } else if (SessionManager::getSession().isProfessor()) {
                string semesterId;
                int semesterNumber = 0;
                getCurrentSemester(db, semesterId, semesterNumber);

                auto courseIds = SessionManager::getAssignedCourseIds(db, semesterId);
                stats.push_back({"assignedCourses", JsonHelper::makeNumber((int)courseIds.size())});

                int totalStudents = 0;
                for (const auto& cid : courseIds) {
                    auto stmt = db.prepareStatement(
                        "SELECT COUNT(*) as cnt FROM registrations "
                        "WHERE course_id = ? AND semester_id = ? AND status = 'registered'"
                    );
                    stmt->setString(1, cid);
                    stmt->setString(2, semesterId);
                    auto r = stmt->executeQuery();
                    if (r->next()) totalStudents += r->getInt("cnt");
                }
                stats.push_back({"totalStudents", JsonHelper::makeNumber(totalStudents)});
            }
            return JsonHelper::makeObject(stats);
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    // ========================================
    // STUDENTS
    // ========================================
    w.bind("cpp_getAllStudents", [](const string& request) -> string {
        try {
            if (!SessionManager::canViewAllStudents()) {
                return "[]";
            }
            
            // Check if user can view passwords
            bool canViewPasswords = SessionManager::hasPermission(Permissions::PWD_VIEW_ALL) ||
                                   SessionManager::hasPermission(Permissions::PWD_VIEW_STU);
            
            // Query students with password if permitted
            string query = 
                "SELECT s.id, s.first_name, s.last_name, s.date_of_birth, s.gender, "
                "u.email, u.phone, s.enrollment_date, s.academic_level_id, s.department_id";
            if (canViewPasswords) {
                query += ", u.password_hash";
            }
            query += " FROM students s LEFT JOIN users u ON s.user_id = u.id ORDER BY s.id";
            
            auto result = db.executeQuery(query);
            vector<string> jsonStudents;
            
            while (result->next()) {
                stringstream ss;
                ss << "{";
                ss << "\"id\":\"" << result->getString("id") << "\",";
                ss << "\"firstName\":\"" << result->getString("first_name") << "\",";
                ss << "\"lastName\":\"" << result->getString("last_name") << "\",";
                ss << "\"fullName\":\"" << result->getString("first_name") << " " << result->getString("last_name") << "\",";
                ss << "\"dateOfBirth\":\"" << result->getString("date_of_birth") << "\",";
                ss << "\"gender\":\"" << result->getString("gender") << "\",";
                ss << "\"email\":\"" << result->getString("email") << "\",";
                ss << "\"phone\":\"" << result->getString("phone") << "\",";
                ss << "\"registrationDate\":\"" << result->getString("enrollment_date") << "\",";
                ss << "\"academicLevelId\":\"" << result->getString("academic_level_id") << "\",";
                ss << "\"departmentId\":\"" << result->getString("department_id") << "\"";
                if (canViewPasswords) {
                    ss << ",\"password\":\"" << result->getString("password_hash") << "\"";
                }
                ss << "}";
                jsonStudents.push_back(ss.str());
            }
            return JsonHelper::makeArray(jsonStudents);
        } catch (const exception& e) {
            cerr << "[Students] Error: " << e.what() << endl;
            return "[]";
        }
    });

    w.bind("cpp_addStudent", [](const string& request) -> string {
        try {
            if (!SessionManager::hasPermission(Permissions::STU_CREATE)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            
            string id = JsonHelper::parseSimpleValue(request, "id");
            string firstName = JsonHelper::parseSimpleValue(request, "firstName");
            string lastName = JsonHelper::parseSimpleValue(request, "lastName");
            string dateOfBirth = JsonHelper::parseSimpleValue(request, "dateOfBirth");
            string gender = JsonHelper::parseSimpleValue(request, "gender");
            string email = JsonHelper::parseSimpleValue(request, "email");
            string phone = JsonHelper::parseSimpleValue(request, "phone");
            string academicLevelId = JsonHelper::parseSimpleValue(request, "academicLevelId");
            string departmentId = JsonHelper::parseSimpleValue(request, "departmentId");

            if (Student::exists(db, id)) {
                return JsonHelper::errorResponse("Student ID already exists!");
            }

            // Normalize gender to lowercase for DB enum
            transform(gender.begin(), gender.end(), gender.begin(),
                      [](unsigned char c) { return static_cast<char>(tolower(c)); });

            Student student(id, firstName, lastName, dateOfBirth, gender,
                           email, phone, academicLevelId, departmentId);
            
            if (student.save(db)) {
                int registeredCount = 0;
                string semesterId;
                int semesterNumber = 0;
                if (getCurrentSemester(db, semesterId, semesterNumber)) {
                    try {
                        db.executeUpdate("START TRANSACTION");
                        auto coursesStmt = db.prepareStatement(
                            "SELECT id FROM courses "
                            "WHERE department_id = ? AND academic_level_id = ? AND semester_number = ?"
                        );
                        coursesStmt->setString(1, departmentId);
                        coursesStmt->setString(2, academicLevelId);
                        coursesStmt->setInt(3, semesterNumber);
                        auto coursesRes = coursesStmt->executeQuery();

                        while (coursesRes->next()) {
                            string courseId = coursesRes->getString("id");
                            auto regStmt = db.prepareStatement(
                                "INSERT INTO registrations (student_id, course_id, semester_id, status, registered_by) "
                                "VALUES (?, ?, ?, 'registered', ?)"
                            );
                            regStmt->setString(1, id);
                            regStmt->setString(2, courseId);
                            regStmt->setString(3, semesterId);
                            regStmt->setString(4, SessionManager::getSession().userId);
                            regStmt->executeUpdate();

                            string regId = to_string(db.getLastInsertId());
                            auto gradeStmt = db.prepareStatement(
                                "INSERT INTO grades (registration_id) VALUES (?)"
                            );
                            gradeStmt->setString(1, regId);
                            gradeStmt->executeUpdate();

                            registeredCount++;
                        }
                        db.executeUpdate("COMMIT");
                    } catch (const exception& e) {
                        try { db.executeUpdate("ROLLBACK"); } catch (...) {}
                        cerr << "[Student] Auto-registration error: " << e.what() << endl;
                    }
                }

                SessionManager::logAction(db, "CREATE", "students", id,
                                          "Added student and registered " + to_string(registeredCount) + " courses");
                return JsonHelper::successResponse("Student added successfully!");
            }
            return JsonHelper::errorResponse("Failed to add student");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_updateStudent", [](const string& request) -> string {
        try {
            if (!SessionManager::hasPermission(Permissions::STU_EDIT)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            
            string id = JsonHelper::parseSimpleValue(request, "id");
            string firstName = JsonHelper::parseSimpleValue(request, "firstName");
            string lastName = JsonHelper::parseSimpleValue(request, "lastName");
            string dateOfBirth = JsonHelper::parseSimpleValue(request, "dateOfBirth");
            string gender = JsonHelper::parseSimpleValue(request, "gender");
            string email = JsonHelper::parseSimpleValue(request, "email");
            string phone = JsonHelper::parseSimpleValue(request, "phone");
            string academicLevelId = JsonHelper::parseSimpleValue(request, "academicLevelId");
            string departmentId = JsonHelper::parseSimpleValue(request, "departmentId");

            transform(gender.begin(), gender.end(), gender.begin(),
                      [](unsigned char c) { return static_cast<char>(tolower(c)); });

            Student student(id, firstName, lastName, dateOfBirth, gender,
                           email, phone, academicLevelId, departmentId);
            
            if (student.update(db)) {
                SessionManager::logAction(db, "UPDATE", "students", id, "Updated student");
                return JsonHelper::successResponse("Student updated successfully!");
            }
            return JsonHelper::errorResponse("Failed to update student");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_deleteStudent", [](const string& request) -> string {
        try {
            if (!SessionManager::hasPermission(Permissions::STU_DELETE)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            
            string id = request;
            id.erase(remove(id.begin(), id.end(), '['), id.end());
            id.erase(remove(id.begin(), id.end(), ']'), id.end());
            id.erase(remove(id.begin(), id.end(), '"'), id.end());

            Student student = Student::getById(db, id);
            if (student.remove(db)) {
                SessionManager::logAction(db, "DELETE", "students", id, "Deleted student");
                return JsonHelper::successResponse("Student deleted!");
            }
            return JsonHelper::errorResponse("Failed to delete student");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_generateStudentId", [](const string& request) -> string {
        try {
            string newId = Student::generateNextId(db);
            vector<pair<string, string>> result;
            result.push_back({"id", JsonHelper::makeString(newId)});
            return JsonHelper::makeObject(result);
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    // ========================================
    // PROFESSORS
    // ========================================
    w.bind("cpp_getAllProfessors", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::PROF_VIEW)) {
                return "[]";
            }
            
            // Check if user can view passwords (only super admin can see professor passwords)
            bool canViewPasswords = SessionManager::hasPermission(Permissions::PWD_VIEW_ALL);
            
            string query = 
                "SELECT p.id, p.first_name, p.last_name, u.email, u.phone, "
                "p.department_id, p.hire_date, p.specialization";
            if (canViewPasswords) {
                query += ", u.password_hash";
            }
            query += " FROM professors p LEFT JOIN users u ON p.user_id = u.id ORDER BY p.id";
            
            auto result = db.executeQuery(query);
            vector<string> jsonProfs;
            
            while (result->next()) {
                stringstream ss;
                ss << "{";
                ss << "\"id\":\"" << result->getString("id") << "\",";
                ss << "\"firstName\":\"" << result->getString("first_name") << "\",";
                ss << "\"lastName\":\"" << result->getString("last_name") << "\",";
                ss << "\"fullName\":\"" << result->getString("first_name") << " " << result->getString("last_name") << "\",";
                ss << "\"email\":\"" << result->getString("email") << "\",";
                ss << "\"phone\":\"" << result->getString("phone") << "\",";
                ss << "\"departmentId\":\"" << result->getString("department_id") << "\",";
                ss << "\"hireDate\":\"" << result->getString("hire_date") << "\",";
                ss << "\"specialization\":\"" << result->getString("specialization") << "\"";
                if (canViewPasswords) {
                    ss << ",\"password\":\"" << result->getString("password_hash") << "\"";
                }
                ss << "}";
                jsonProfs.push_back(ss.str());
            }
            return JsonHelper::makeArray(jsonProfs);
        } catch (const exception& e) {
            cerr << "[Professors] Error: " << e.what() << endl;
            return "[]";
        }
    });

    w.bind("cpp_addProfessor", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::PROF_CREATE)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            string id = JsonHelper::parseSimpleValue(request, "id");
            string firstName = JsonHelper::parseSimpleValue(request, "firstName");
            string lastName = JsonHelper::parseSimpleValue(request, "lastName");
            string email = JsonHelper::parseSimpleValue(request, "email");
            string phone = JsonHelper::parseSimpleValue(request, "phone");
            string departmentId = JsonHelper::parseSimpleValue(request, "departmentId");
            string specialization = JsonHelper::parseSimpleValue(request, "specialization");

            if (Professor::exists(db, id)) {
                return JsonHelper::errorResponse("Professor ID already exists!");
            }

            Professor prof(id, firstName, lastName, email, phone, departmentId, specialization);
            if (prof.save(db)) {
                SessionManager::logAction(db, "CREATE", "professors", id, "Added professor");
                return JsonHelper::successResponse("Professor added!");
            }
            return JsonHelper::errorResponse("Failed to add professor");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_updateProfessor", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::PROF_EDIT)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            string id = JsonHelper::parseSimpleValue(request, "id");
            string firstName = JsonHelper::parseSimpleValue(request, "firstName");
            string lastName = JsonHelper::parseSimpleValue(request, "lastName");
            string email = JsonHelper::parseSimpleValue(request, "email");
            string phone = JsonHelper::parseSimpleValue(request, "phone");
            string departmentId = JsonHelper::parseSimpleValue(request, "departmentId");
            string specialization = JsonHelper::parseSimpleValue(request, "specialization");

            if (!Professor::exists(db, id)) {
                return JsonHelper::errorResponse("Professor not found");
            }

            Professor prof(id, firstName, lastName, email, phone, departmentId, specialization);
            if (prof.update(db)) {
                SessionManager::logAction(db, "UPDATE", "professors", id, "Updated professor");
                return JsonHelper::successResponse("Professor updated!");
            }
            return JsonHelper::errorResponse("Failed to update professor");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_deleteProfessor", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::PROF_DELETE)) {
                return JsonHelper::errorResponse("Permission denied");
            }

            string id = request;
            id.erase(remove(id.begin(), id.end(), '['), id.end());
            id.erase(remove(id.begin(), id.end(), ']'), id.end());
            id.erase(remove(id.begin(), id.end(), '"'), id.end());

            if (!Professor::exists(db, id)) {
                return JsonHelper::errorResponse("Professor not found");
            }

            Professor prof = Professor::getById(db, id);
            if (prof.remove(db)) {
                SessionManager::logAction(db, "DELETE", "professors", id, "Deleted professor");
                return JsonHelper::successResponse("Professor deleted!");
            }
            return JsonHelper::errorResponse("Failed to delete professor");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    // ========================================
    // COURSES
    // ========================================
    w.bind("cpp_getAllCourses", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::CRS_VIEW)) {
                return "[]";
            }
            const UserSession& session = SessionManager::getSession();
            vector<string> jsonCourses;

            string semesterId = JsonHelper::parseSimpleValue(request, "semesterId");
            if (session.isProfessor() && !session.linkedId.empty()) {
                if (semesterId.empty()) {
                    int semesterNumber = 0;
                    getCurrentSemester(db, semesterId, semesterNumber);
                }
            }

            string baseQuery =
                "SELECT c.id, c.code, c.name_en, c.name_ar, c.description, "
                "c.department_id, d.name_en AS department_name, "
                "c.academic_level_id, al.name_en AS level_name, "
                "c.semester_number, c.course_type_id, ct.name AS course_type_name, "
                "c.credit_hours, c.lecture_hours, c.tutorial_hours, c.lab_hours, c.practical_hours, "
                "c.max_marks, c.year_work_marks, c.assignment1_marks, c.assignment2_marks, c.final_exam_marks "
                "FROM courses c "
                "JOIN departments d ON c.department_id = d.id "
                "JOIN academic_levels al ON c.academic_level_id = al.id "
                "JOIN course_types ct ON c.course_type_id = ct.id ";

            if (session.isProfessor() && !session.linkedId.empty()) {
                baseQuery += "JOIN course_assignments ca ON c.id = ca.course_id "
                             "WHERE ca.professor_id = ? ";
                if (!semesterId.empty()) {
                    baseQuery += "AND ca.semester_id = ? ";
                }
                baseQuery += "ORDER BY c.code";

                auto stmt = db.prepareStatement(baseQuery);
                stmt->setString(1, session.linkedId);
                if (!semesterId.empty()) {
                    stmt->setString(2, semesterId);
                }
                auto result = stmt->executeQuery();

                while (result->next()) {
                    stringstream ss;
                    ss << "{";
                    ss << "\"id\":\"" << result->getString("id") << "\",";
                    ss << "\"code\":\"" << result->getString("code") << "\",";
                    ss << "\"nameEn\":\"" << result->getString("name_en") << "\",";
                    ss << "\"nameAr\":\"" << result->getString("name_ar") << "\",";
                    ss << "\"description\":\"" << result->getString("description") << "\",";
                    ss << "\"departmentId\":\"" << result->getString("department_id") << "\",";
                    ss << "\"departmentName\":\"" << result->getString("department_name") << "\",";
                    ss << "\"academicLevelId\":\"" << result->getString("academic_level_id") << "\",";
                    ss << "\"academicLevelName\":\"" << result->getString("level_name") << "\",";
                    ss << "\"semesterNumber\":" << result->getInt("semester_number") << ",";
                    ss << "\"courseTypeId\":\"" << result->getString("course_type_id") << "\",";
                    ss << "\"courseTypeName\":\"" << result->getString("course_type_name") << "\",";
                    ss << "\"creditHours\":" << result->getInt("credit_hours") << ",";
                    ss << "\"lectureHours\":" << result->getInt("lecture_hours") << ",";
                    ss << "\"tutorialHours\":" << result->getInt("tutorial_hours") << ",";
                    ss << "\"labHours\":" << result->getInt("lab_hours") << ",";
                    ss << "\"practicalHours\":" << result->getInt("practical_hours") << ",";
                    ss << "\"maxMarks\":" << result->getInt("max_marks") << ",";
                    ss << "\"yearWorkMarks\":" << result->getInt("year_work_marks") << ",";
                    ss << "\"assignment1Marks\":" << result->getInt("assignment1_marks") << ",";
                    ss << "\"assignment2Marks\":" << result->getInt("assignment2_marks") << ",";
                    ss << "\"finalExamMarks\":" << result->getInt("final_exam_marks");
                    ss << "}";
                    jsonCourses.push_back(ss.str());
                }
            } else {
                baseQuery += "ORDER BY c.code";
                auto result = db.executeQuery(baseQuery);
                while (result->next()) {
                    stringstream ss;
                    ss << "{";
                    ss << "\"id\":\"" << result->getString("id") << "\",";
                    ss << "\"code\":\"" << result->getString("code") << "\",";
                    ss << "\"nameEn\":\"" << result->getString("name_en") << "\",";
                    ss << "\"nameAr\":\"" << result->getString("name_ar") << "\",";
                    ss << "\"description\":\"" << result->getString("description") << "\",";
                    ss << "\"departmentId\":\"" << result->getString("department_id") << "\",";
                    ss << "\"departmentName\":\"" << result->getString("department_name") << "\",";
                    ss << "\"academicLevelId\":\"" << result->getString("academic_level_id") << "\",";
                    ss << "\"academicLevelName\":\"" << result->getString("level_name") << "\",";
                    ss << "\"semesterNumber\":" << result->getInt("semester_number") << ",";
                    ss << "\"courseTypeId\":\"" << result->getString("course_type_id") << "\",";
                    ss << "\"courseTypeName\":\"" << result->getString("course_type_name") << "\",";
                    ss << "\"creditHours\":" << result->getInt("credit_hours") << ",";
                    ss << "\"lectureHours\":" << result->getInt("lecture_hours") << ",";
                    ss << "\"tutorialHours\":" << result->getInt("tutorial_hours") << ",";
                    ss << "\"labHours\":" << result->getInt("lab_hours") << ",";
                    ss << "\"practicalHours\":" << result->getInt("practical_hours") << ",";
                    ss << "\"maxMarks\":" << result->getInt("max_marks") << ",";
                    ss << "\"yearWorkMarks\":" << result->getInt("year_work_marks") << ",";
                    ss << "\"assignment1Marks\":" << result->getInt("assignment1_marks") << ",";
                    ss << "\"assignment2Marks\":" << result->getInt("assignment2_marks") << ",";
                    ss << "\"finalExamMarks\":" << result->getInt("final_exam_marks");
                    ss << "}";
                    jsonCourses.push_back(ss.str());
                }
            }
            return JsonHelper::makeArray(jsonCourses);
        } catch (...) {
            return "[]";
        }
    });

    w.bind("cpp_getCourseAssignments", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::CRS_VIEW)) {
                return "[]";
            }
            string semesterId = JsonHelper::parseSimpleValue(request, "semesterId");

            string query =
                "SELECT ca.id, ca.course_id, c.code AS course_code, c.name_en AS course_name, "
                "ca.professor_id, CONCAT(p.first_name, ' ', p.last_name) AS professor_name, "
                "ca.semester_id, sem.name_en AS semester_name, ca.is_primary "
                "FROM course_assignments ca "
                "JOIN courses c ON ca.course_id = c.id "
                "JOIN professors p ON ca.professor_id = p.id "
                "JOIN semesters sem ON ca.semester_id = sem.id ";
            if (!semesterId.empty()) {
                query += "WHERE ca.semester_id = ? ";
            }
            query += "ORDER BY sem.academic_year DESC, c.code";

            vector<string> assignments;
            if (!semesterId.empty()) {
                auto stmt = db.prepareStatement(query);
                stmt->setString(1, semesterId);
                auto result = stmt->executeQuery();
                while (result->next()) {
                    vector<pair<string, string>> item;
                    item.push_back({"id", JsonHelper::makeString(result->getString("id"))});
                    item.push_back({"courseId", JsonHelper::makeString(result->getString("course_id"))});
                    item.push_back({"courseCode", JsonHelper::makeString(result->getString("course_code"))});
                    item.push_back({"courseName", JsonHelper::makeString(result->getString("course_name"))});
                    item.push_back({"professorId", JsonHelper::makeString(result->getString("professor_id"))});
                    item.push_back({"professorName", JsonHelper::makeString(result->getString("professor_name"))});
                    item.push_back({"semesterId", JsonHelper::makeString(result->getString("semester_id"))});
                    item.push_back({"semesterName", JsonHelper::makeString(result->getString("semester_name"))});
                    item.push_back({"isPrimary", JsonHelper::makeBool(result->getBoolean("is_primary"))});
                    assignments.push_back(JsonHelper::makeObject(item));
                }
            } else {
                auto result = db.executeQuery(query);
                while (result->next()) {
                    vector<pair<string, string>> item;
                    item.push_back({"id", JsonHelper::makeString(result->getString("id"))});
                    item.push_back({"courseId", JsonHelper::makeString(result->getString("course_id"))});
                    item.push_back({"courseCode", JsonHelper::makeString(result->getString("course_code"))});
                    item.push_back({"courseName", JsonHelper::makeString(result->getString("course_name"))});
                    item.push_back({"professorId", JsonHelper::makeString(result->getString("professor_id"))});
                    item.push_back({"professorName", JsonHelper::makeString(result->getString("professor_name"))});
                    item.push_back({"semesterId", JsonHelper::makeString(result->getString("semester_id"))});
                    item.push_back({"semesterName", JsonHelper::makeString(result->getString("semester_name"))});
                    item.push_back({"isPrimary", JsonHelper::makeBool(result->getBoolean("is_primary"))});
                    assignments.push_back(JsonHelper::makeObject(item));
                }
            }
            return JsonHelper::makeArray(assignments);
        } catch (...) {
            return "[]";
        }
    });

    w.bind("cpp_assignCourse", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::CRS_ASSIGN)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            string courseId = JsonHelper::parseSimpleValue(request, "courseId");
            string professorId = JsonHelper::parseSimpleValue(request, "professorId");
            string semesterId = JsonHelper::parseSimpleValue(request, "semesterId");
            string isPrimaryStr = JsonHelper::parseSimpleValue(request, "isPrimary");
            bool isPrimary = isPrimaryStr == "true" || isPrimaryStr == "1";

            auto existsStmt = db.prepareStatement(
                "SELECT COUNT(*) as cnt FROM course_assignments "
                "WHERE course_id = ? AND professor_id = ? AND semester_id = ?"
            );
            existsStmt->setString(1, courseId);
            existsStmt->setString(2, professorId);
            existsStmt->setString(3, semesterId);
            auto existsRes = existsStmt->executeQuery();
            if (existsRes->next() && existsRes->getInt("cnt") > 0) {
                return JsonHelper::errorResponse("Assignment already exists");
            }

            auto stmt = db.prepareStatement(
                "INSERT INTO course_assignments (course_id, professor_id, semester_id, is_primary, assigned_by) "
                "VALUES (?, ?, ?, ?, ?)"
            );
            stmt->setString(1, courseId);
            stmt->setString(2, professorId);
            stmt->setString(3, semesterId);
            stmt->setInt(4, isPrimary ? 1 : 0);
            stmt->setString(5, SessionManager::getSession().userId);
            stmt->executeUpdate();

            SessionManager::logAction(db, "ASSIGN", "course_assignments",
                                      to_string(db.getLastInsertId()),
                                      "Assigned course " + courseId + " to " + professorId);
            return JsonHelper::successResponse("Assignment saved!");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_removeCourseAssignment", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::CRS_ASSIGN)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            string assignmentId = request;
            assignmentId.erase(remove(assignmentId.begin(), assignmentId.end(), '['), assignmentId.end());
            assignmentId.erase(remove(assignmentId.begin(), assignmentId.end(), ']'), assignmentId.end());
            assignmentId.erase(remove(assignmentId.begin(), assignmentId.end(), '\"'), assignmentId.end());

            auto stmt = db.prepareStatement("DELETE FROM course_assignments WHERE id = ?");
            stmt->setString(1, assignmentId);
            int rows = stmt->executeUpdate();
            if (rows > 0) {
                SessionManager::logAction(db, "DELETE", "course_assignments", assignmentId,
                                          "Removed course assignment");
                return JsonHelper::successResponse("Assignment removed!");
            }
            return JsonHelper::errorResponse("Assignment not found");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    // ========================================
    // REGISTRATIONS
    // ========================================
    w.bind("cpp_getAllRegistrations", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::REG_VIEW)) {
                return "[]";
            }
            auto result = db.executeQuery(
                "SELECT r.*, CONCAT(s.first_name, ' ', s.last_name) AS student_name, "
                "c.code AS course_code, c.name_en AS course_name, "
                "sem.name_en AS semester_name "
                "FROM registrations r "
                "JOIN students s ON r.student_id = s.id "
                "JOIN courses c ON r.course_id = c.id "
                "JOIN semesters sem ON r.semester_id = sem.id "
                "ORDER BY r.created_at DESC"
            );

            vector<string> registrations;
            while (result->next()) {
                vector<pair<string, string>> reg;
                reg.push_back({"id", JsonHelper::makeString(result->getString("id"))});
                reg.push_back({"studentId", JsonHelper::makeString(result->getString("student_id"))});
                reg.push_back({"studentName", JsonHelper::makeString(result->getString("student_name"))});
                reg.push_back({"courseId", JsonHelper::makeString(result->getString("course_id"))});
                reg.push_back({"courseCode", JsonHelper::makeString(result->getString("course_code"))});
                reg.push_back({"courseName", JsonHelper::makeString(result->getString("course_name"))});
                reg.push_back({"semesterId", JsonHelper::makeString(result->getString("semester_id"))});
                reg.push_back({"semesterName", JsonHelper::makeString(result->getString("semester_name"))});
                reg.push_back({"status", JsonHelper::makeString(result->getString("status"))});
                reg.push_back({"registrationDate", JsonHelper::makeString(result->getString("created_at"))});
                registrations.push_back(JsonHelper::makeObject(reg));
            }
            return JsonHelper::makeArray(registrations);
        } catch (...) {
            return "[]";
        }
    });

    w.bind("cpp_addRegistration", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::REG_CREATE)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            string studentId = JsonHelper::parseSimpleValue(request, "studentId");
            string courseId = JsonHelper::parseSimpleValue(request, "courseId");
            string semesterId = JsonHelper::parseSimpleValue(request, "semesterId");

            auto checkStmt = db.prepareStatement(
                "SELECT COUNT(*) as count FROM registrations "
                "WHERE student_id = ? AND course_id = ? AND semester_id = ?"
            );
            checkStmt->setString(1, studentId);
            checkStmt->setString(2, courseId);
            checkStmt->setString(3, semesterId);
            auto checkResult = checkStmt->executeQuery();
            
            if (checkResult->next() && checkResult->getInt("count") > 0) {
                return JsonHelper::errorResponse("Student already registered for this course!");
            }

            auto stmt = db.prepareStatement(
                "INSERT INTO registrations (student_id, course_id, semester_id, status, registered_by) "
                "VALUES (?, ?, ?, 'registered', ?)"
            );
            stmt->setString(1, studentId);
            stmt->setString(2, courseId);
            stmt->setString(3, semesterId);
            stmt->setString(4, SessionManager::getSession().userId);
            stmt->executeUpdate();

            string regId = to_string(db.getLastInsertId());

            // Create grade record
            auto gradeStmt = db.prepareStatement("INSERT INTO grades (registration_id) VALUES (?)");
            gradeStmt->setString(1, regId);
            gradeStmt->executeUpdate();

            SessionManager::logAction(db, "CREATE", "registrations", regId,
                                      "Registered " + studentId + " to " + courseId);
            return JsonHelper::successResponse("Registration successful!");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    // ========================================
    // GRADES
    // ========================================
    w.bind("cpp_getStudentsByCourse", [](const string& request) -> string {
        try {
            string courseId = JsonHelper::parseSimpleValue(request, "courseId");
            string semesterId = JsonHelper::parseSimpleValue(request, "semesterId");
            if (semesterId.empty()) {
                int semesterNumber = 0;
                getCurrentSemester(db, semesterId, semesterNumber);
            }

            if (!SessionManager::isLoggedIn()) {
                return "[]";
            }

            const UserSession& session = SessionManager::getSession();
            if (session.isProfessor()) {
                if (!SessionManager::canViewAssignedCourses() && !SessionManager::canEnterGrades()) {
                    return "[]";
                }
                if (!SessionManager::canAccessCourse(db, courseId, semesterId)) {
                    return "[]";
                }
            } else if (!SessionManager::canViewAllGrades()) {
                return "[]";
            }

            auto recalcStmt = db.prepareStatement(
                "SELECT r.id FROM registrations r "
                "WHERE r.course_id = ? AND r.semester_id = ? AND r.status = 'registered'"
            );
            recalcStmt->setString(1, courseId);
            recalcStmt->setString(2, semesterId);
            auto recalcRes = recalcStmt->executeQuery();
            while (recalcRes->next()) {
                Grade::calculateGrade(db, recalcRes->getString("id"));
            }

            auto stmt = db.prepareStatement(
                "SELECT r.id AS registration_id, r.student_id, "
                "CONCAT(s.first_name, ' ', s.last_name) AS student_name, "
                "COALESCE(g.assignment1, 0) AS assignment1, "
                "COALESCE(g.assignment2, 0) AS assignment2, "
                "COALESCE(g.year_work, 0) AS year_work, "
                "COALESCE(g.final_exam, 0) AS final_exam, "
                "COALESCE(g.total_marks, 0) AS total_marks, "
                "COALESCE(g.percentage, 0) AS percentage, "
                "COALESCE(g.gpa, 0) AS gpa, "
                "COALESCE(g.evaluation, 'Fail') AS evaluation, "
                "COALESCE(g.letter_grade, 'F') AS letter_grade "
                "FROM registrations r "
                "JOIN students s ON r.student_id = s.id "
                "LEFT JOIN grades g ON r.id = g.registration_id "
                "WHERE r.course_id = ? AND r.semester_id = ? AND r.status = 'registered' "
                "ORDER BY s.first_name"
            );
            stmt->setString(1, courseId);
            stmt->setString(2, semesterId);

            auto result = stmt->executeQuery();
            vector<string> students;
            
            while (result->next()) {
                vector<pair<string, string>> student;
                student.push_back({"registrationId", JsonHelper::makeString(result->getString("registration_id"))});
                student.push_back({"studentId", JsonHelper::makeString(result->getString("student_id"))});
                student.push_back({"studentName", JsonHelper::makeString(result->getString("student_name"))});
                student.push_back({"assignment1", JsonHelper::makeNumber(result->getDouble("assignment1"))});
                student.push_back({"assignment2", JsonHelper::makeNumber(result->getDouble("assignment2"))});
                student.push_back({"yearWork", JsonHelper::makeNumber(result->getDouble("year_work"))});
                student.push_back({"finalExam", JsonHelper::makeNumber(result->getDouble("final_exam"))});
                student.push_back({"total", JsonHelper::makeNumber(result->getDouble("total_marks"))});
                student.push_back({"percentage", JsonHelper::makeNumber(result->getDouble("percentage"))});
                student.push_back({"gpa", JsonHelper::makeNumber(result->getDouble("gpa"))});
                student.push_back({"evaluation", JsonHelper::makeString(result->getString("evaluation"))});
                student.push_back({"letterGrade", JsonHelper::makeString(result->getString("letter_grade"))});
                students.push_back(JsonHelper::makeObject(student));
            }
            return JsonHelper::makeArray(students);
        } catch (...) {
            return "[]";
        }
    });

    w.bind("cpp_enterGrade", [](const string& request) -> string {
        try {
            if (!SessionManager::canEnterGrades()) {
                return JsonHelper::errorResponse("Permission denied: Only professors can enter grades");
            }

            string registrationId = JsonHelper::parseSimpleValue(request, "registrationId");
            string gradeType = JsonHelper::parseSimpleValue(request, "gradeType");
            double value = stod(JsonHelper::parseSimpleValue(request, "value"));

            if (gradeType != "assignment1" && gradeType != "assignment2" &&
                gradeType != "year_work" && gradeType != "final_exam") {
                return JsonHelper::errorResponse("Invalid grade type");
            }

            string validationError = Grade::validateGradeComponent(db, registrationId, gradeType, value);
            if (!validationError.empty()) {
                return JsonHelper::errorResponse(validationError);
            }

            // Professor must be assigned to the course for this registration
            if (SessionManager::getSession().isProfessor()) {
                auto infoStmt = db.prepareStatement(
                    "SELECT course_id, semester_id FROM registrations WHERE id = ?"
                );
                infoStmt->setString(1, registrationId);
                auto infoRes = infoStmt->executeQuery();
                if (!infoRes->next()) {
                    return JsonHelper::errorResponse("Registration not found");
                }
                if (!SessionManager::canAccessCourse(db,
                                                    infoRes->getString("course_id"),
                                                    infoRes->getString("semester_id"))) {
                    return JsonHelper::errorResponse("Permission denied for this course");
                }
            }

            string query = "UPDATE grades SET " + gradeType + " = ?, last_modified_by = ? WHERE registration_id = ?";
            auto stmt = db.prepareStatement(query);
            stmt->setDouble(1, value);
            stmt->setString(2, SessionManager::getSession().userId);
            stmt->setString(3, registrationId);
            stmt->executeUpdate();

            Grade::calculateGrade(db, registrationId);
            SessionManager::logAction(db, "UPDATE_GRADE", "grades", registrationId, gradeType + " = " + to_string(value));

            return JsonHelper::successResponse("Grade updated!");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    // ========================================
    // ATTENDANCE
    // ========================================
    w.bind("cpp_getAttendanceByDate", [](const string& request) -> string {
        try {
            string courseId = JsonHelper::parseSimpleValue(request, "courseId");
            string semesterId = JsonHelper::parseSimpleValue(request, "semesterId");
            string date = JsonHelper::parseSimpleValue(request, "date");
            string sessionType = JsonHelper::parseSimpleValue(request, "sessionType");

            if (!SessionManager::isLoggedIn()) {
                return "[]";
            }
            const UserSession& session = SessionManager::getSession();
            if (session.isProfessor()) {
                if (!SessionManager::canRecordAttendance()) {
                    return "[]";
                }
                if (!SessionManager::canAccessCourse(db, courseId, semesterId)) {
                    return "[]";
                }
            } else if (!SessionManager::hasPermission(Permissions::ATT_VIEW_ALL)) {
                return "[]";
            }

            string sessionId;
            auto sessionStmt = db.prepareStatement(
                "SELECT id FROM sessions WHERE course_id = ? AND semester_id = ? "
                "AND session_date = ? AND session_type = ?"
            );
            sessionStmt->setString(1, courseId);
            sessionStmt->setString(2, semesterId);
            sessionStmt->setString(3, date);
            sessionStmt->setString(4, sessionType);
            auto sessionRes = sessionStmt->executeQuery();
            if (sessionRes->next()) {
                sessionId = sessionRes->getString("id");
            }
            string sessionJoinId = sessionId.empty() ? "0" : sessionId;

            auto stmt = db.prepareStatement(
                "SELECT r.id AS registration_id, r.student_id, "
                "CONCAT(s.first_name, ' ', s.last_name) AS student_name, "
                "a.id AS attendance_id, a.status, a.remarks "
                "FROM registrations r "
                "JOIN students s ON r.student_id = s.id "
                "LEFT JOIN attendance a ON a.student_id = r.student_id AND a.session_id = ? "
                "WHERE r.course_id = ? AND r.semester_id = ? AND r.status = 'registered' "
                "ORDER BY s.first_name"
            );
            stmt->setString(1, sessionJoinId);
            stmt->setString(2, courseId);
            stmt->setString(3, semesterId);

            auto result = stmt->executeQuery();
            vector<string> students;

            while (result->next()) {
                vector<pair<string, string>> student;
                student.push_back({"registrationId", JsonHelper::makeString(result->getString("registration_id"))});
                student.push_back({"studentId", JsonHelper::makeString(result->getString("student_id"))});
                student.push_back({"studentName", JsonHelper::makeString(result->getString("student_name"))});
                student.push_back({"attendanceId", JsonHelper::makeString(result->getString("attendance_id"))});
                student.push_back({"status", JsonHelper::makeString(result->getString("status"))});
                student.push_back({"remarks", JsonHelper::makeString(result->getString("remarks"))});
                students.push_back(JsonHelper::makeObject(student));
            }
            return JsonHelper::makeArray(students);
        } catch (...) {
            return "[]";
        }
    });

    w.bind("cpp_saveAttendance", [](const string& request) -> string {
        try {
            if (!SessionManager::canRecordAttendance()) {
                return JsonHelper::errorResponse("Permission denied");
            }

            string courseId = JsonHelper::parseSimpleValue(request, "courseId");
            string semesterId = JsonHelper::parseSimpleValue(request, "semesterId");
            string studentId = JsonHelper::parseSimpleValue(request, "studentId");
            string date = JsonHelper::parseSimpleValue(request, "date");
            string sessionType = JsonHelper::parseSimpleValue(request, "sessionType");
            string status = JsonHelper::parseSimpleValue(request, "status");
            string remarks = JsonHelper::parseSimpleValue(request, "remarks");

            // Professor must be assigned to the course for this semester
            if (SessionManager::getSession().isProfessor()) {
                if (!SessionManager::canAccessCourse(db, courseId, semesterId)) {
                    return JsonHelper::errorResponse("Permission denied for this course");
                }
            }

            // Ensure session exists
            string sessionId;
            auto sessionStmt = db.prepareStatement(
                "SELECT id FROM sessions WHERE course_id = ? AND semester_id = ? "
                "AND session_date = ? AND session_type = ?"
            );
            sessionStmt->setString(1, courseId);
            sessionStmt->setString(2, semesterId);
            sessionStmt->setString(3, date);
            sessionStmt->setString(4, sessionType);
            auto sessionRes = sessionStmt->executeQuery();
            if (sessionRes->next()) {
                sessionId = sessionRes->getString("id");
            } else {
                auto createStmt = db.prepareStatement(
                    "INSERT INTO sessions (course_id, professor_id, semester_id, session_date, session_type) "
                    "VALUES (?, ?, ?, ?, ?)"
                );
                createStmt->setString(1, courseId);
                if (SessionManager::getSession().isProfessor()) {
                    createStmt->setString(2, SessionManager::getSession().linkedId);
                } else {
                    createStmt->setNull(2);
                }
                createStmt->setString(3, semesterId);
                createStmt->setString(4, date);
                createStmt->setString(5, sessionType);
                createStmt->executeUpdate();
                sessionId = to_string(db.getLastInsertId());
            }

            auto checkStmt = db.prepareStatement(
                "SELECT id FROM attendance WHERE session_id = ? AND student_id = ?"
            );
            checkStmt->setString(1, sessionId);
            checkStmt->setString(2, studentId);
            auto checkResult = checkStmt->executeQuery();

            if (checkResult->next()) {
                auto updateStmt = db.prepareStatement(
                    "UPDATE attendance SET status = ?, remarks = ?, recorded_by = ? WHERE id = ?"
                );
                updateStmt->setString(1, status);
                updateStmt->setString(2, remarks);
                updateStmt->setString(3, SessionManager::getSession().userId);
                updateStmt->setString(4, checkResult->getString("id"));
                updateStmt->executeUpdate();
            } else {
                auto insertStmt = db.prepareStatement(
                    "INSERT INTO attendance (session_id, student_id, status, remarks, recorded_by) "
                    "VALUES (?, ?, ?, ?, ?)"
                );
                insertStmt->setString(1, sessionId);
                insertStmt->setString(2, studentId);
                insertStmt->setString(3, status);
                insertStmt->setString(4, remarks);
                insertStmt->setString(5, SessionManager::getSession().userId);
                insertStmt->executeUpdate();
            }
            return JsonHelper::successResponse("Attendance saved!");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    // ========================================
    // FEES MANAGEMENT
    // ========================================
    w.bind("cpp_getStudentFees", [](const string& request) -> string {
        try {
            if (!SessionManager::hasPermission(Permissions::FEE_VIEW)) {
                return "[]";
            }
            
            string studentId = JsonHelper::parseSimpleValue(request, "studentId");
            string semesterId = JsonHelper::parseSimpleValue(request, "semesterId");
            
            string query = 
                "SELECT sf.*, "
                "CONCAT(s.first_name, ' ', s.last_name) AS student_name, "
                "s.id AS student_id, "
                "sem.name_en AS semester_name "
                "FROM student_fees sf "
                "JOIN students s ON sf.student_id = s.id "
                "JOIN semesters sem ON sf.semester_id = sem.id "
                "WHERE 1=1 ";
            
            vector<string> params;
            if (!studentId.empty()) {
                query += "AND sf.student_id = ? ";
                params.push_back(studentId);
            }
            if (!semesterId.empty()) {
                query += "AND sf.semester_id = ? ";
                params.push_back(semesterId);
            }
            query += "ORDER BY sf.academic_year DESC, sf.created_at DESC";
            
            auto stmt = db.prepareStatement(query);
            for (size_t i = 0; i < params.size(); i++) {
                stmt->setString(static_cast<int>(i + 1), params[i]);
            }
            auto result = stmt->executeQuery();
            
            vector<string> fees;
            while (result->next()) {
                stringstream ss;
                ss << "{";
                ss << "\"id\"," << result->getInt("id") << ",";
                ss << "\"studentId\":\"" << result->getString("student_id") << "\",";
                ss << "\"studentName\":\"" << result->getString("student_name") << "\",";
                ss << "\"academicYear\":\"" << result->getString("academic_year") << "\",";
                ss << "\"semesterId\":\"" << result->getString("semester_id") << "\",";
                ss << "\"semesterName\":\"" << result->getString("semester_name") << "\",";
                ss << "\"baseFee\":" << result->getDouble("base_fee") << ",";
                ss << "\"additionalFees\":" << result->getDouble("additional_fees") << ",";
                ss << "\"discount\":" << result->getDouble("discount") << ",";
                ss << "\"totalFees\":" << result->getDouble("total_fees") << ",";
                ss << "\"paidAmount\":" << result->getDouble("paid_amount") << ",";
                ss << "\"remainingAmount\":" << result->getDouble("remaining_amount") << ",";
                ss << "\"status\":\"" << result->getString("status") << "\",";
                ss << "\"dueDate\":\"" << (result->isNull("due_date") ? "" : result->getString("due_date")) << "\",";
                ss << "\"notes\":\"" << (result->isNull("notes") ? "" : result->getString("notes")) << "\"";
                ss << "}";
                fees.push_back(ss.str());
            }
            return JsonHelper::makeArray(fees);
        } catch (const exception& e) {
            cerr << "[Fees] Error: " << e.what() << endl;
            return "[]";
        }
    });

    w.bind("cpp_createStudentFee", [](const string& request) -> string {
        try {
            if (!SessionManager::hasPermission(Permissions::FEE_CREATE)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            
            string studentId = JsonHelper::parseSimpleValue(request, "studentId");
            string academicYear = JsonHelper::parseSimpleValue(request, "academicYear");
            string semesterId = JsonHelper::parseSimpleValue(request, "semesterId");
            string baseFee = JsonHelper::parseSimpleValue(request, "baseFee");
            string additionalFees = JsonHelper::parseSimpleValue(request, "additionalFees");
            string discount = JsonHelper::parseSimpleValue(request, "discount");
            string dueDate = JsonHelper::parseSimpleValue(request, "dueDate");
            string notes = JsonHelper::parseSimpleValue(request, "notes");
            
            // Validate required fields
            if (studentId.empty() || semesterId.empty() || academicYear.empty()) {
                return JsonHelper::errorResponse("Student ID, Semester, and Academic Year are required");
            }
            
            // Validate and parse numeric values with error handling
            double baseFeeValue = 15000.0;
            double additionalFeesValue = 0.0;
            double discountValue = 0.0;
            
            try {
                if (!baseFee.empty()) {
                    baseFeeValue = stod(baseFee);
                    if (baseFeeValue < 0) {
                        return JsonHelper::errorResponse("Base fee cannot be negative");
                    }
                }
                
                if (!additionalFees.empty()) {
                    additionalFeesValue = stod(additionalFees);
                    if (additionalFeesValue < 0) {
                        return JsonHelper::errorResponse("Additional fees cannot be negative");
                    }
                }
                
                if (!discount.empty()) {
                    discountValue = stod(discount);
                    if (discountValue < 0) {
                        return JsonHelper::errorResponse("Discount cannot be negative");
                    }
                }
            } catch (const invalid_argument&) {
                return JsonHelper::errorResponse("Invalid numeric value for fees");
            } catch (const out_of_range&) {
                return JsonHelper::errorResponse("Fee value out of range");
            }
            
            auto stmt = db.prepareStatement(
                "INSERT INTO student_fees (student_id, academic_year, semester_id, base_fee, "
                "additional_fees, discount, due_date, notes) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?)"
            );
            stmt->setString(1, studentId);
            stmt->setString(2, academicYear);
            stmt->setString(3, semesterId);
            stmt->setDouble(4, baseFeeValue);
            stmt->setDouble(5, additionalFeesValue);
            stmt->setDouble(6, discountValue);
            if (dueDate.empty()) stmt->setNull(7); else stmt->setString(7, dueDate);
            if (notes.empty()) stmt->setNull(8); else stmt->setString(8, notes);
            
            stmt->executeUpdate();
            
            SessionManager::logAction(db, "CREATE", "student_fees", 
                                      to_string(db.getLastInsertId()), 
                                      "Created fee record for student " + studentId);
            
            return JsonHelper::successResponse("Fee record created successfully!");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_updateStudentFee", [](const string& request) -> string {
        try {
            if (!SessionManager::hasPermission(Permissions::FEE_EDIT)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            
            string feeId = JsonHelper::parseSimpleValue(request, "id");
            string baseFee = JsonHelper::parseSimpleValue(request, "baseFee");
            string additionalFees = JsonHelper::parseSimpleValue(request, "additionalFees");
            string discount = JsonHelper::parseSimpleValue(request, "discount");
            string dueDate = JsonHelper::parseSimpleValue(request, "dueDate");
            string notes = JsonHelper::parseSimpleValue(request, "notes");
            
            // Validate required fields
            if (feeId.empty()) {
                return JsonHelper::errorResponse("Fee ID is required");
            }
            
            // Validate and parse numeric values with defaults
            int feeIdValue = 0;
            double baseFeeValue = 0.0;
            double additionalFeesValue = 0.0;
            double discountValue = 0.0;
            
            try {
                feeIdValue = stoi(feeId);
                
                if (!baseFee.empty()) {
                    baseFeeValue = stod(baseFee);
                    if (baseFeeValue < 0) {
                        return JsonHelper::errorResponse("Base fee cannot be negative");
                    }
                }
                
                if (!additionalFees.empty()) {
                    additionalFeesValue = stod(additionalFees);
                    if (additionalFeesValue < 0) {
                        return JsonHelper::errorResponse("Additional fees cannot be negative");
                    }
                }
                
                if (!discount.empty()) {
                    discountValue = stod(discount);
                    if (discountValue < 0) {
                        return JsonHelper::errorResponse("Discount cannot be negative");
                    }
                }
            } catch (const invalid_argument&) {
                return JsonHelper::errorResponse("Invalid numeric value");
            } catch (const out_of_range&) {
                return JsonHelper::errorResponse("Value out of range");
            }
            
            auto stmt = db.prepareStatement(
                "UPDATE student_fees SET base_fee = ?, additional_fees = ?, "
                "discount = ?, due_date = ?, notes = ? WHERE id = ?"
            );
            stmt->setDouble(1, baseFeeValue);
            stmt->setDouble(2, additionalFeesValue);
            stmt->setDouble(3, discountValue);
            if (dueDate.empty()) stmt->setNull(4); else stmt->setString(4, dueDate);
            if (notes.empty()) stmt->setNull(5); else stmt->setString(5, notes);
            stmt->setInt(6, feeIdValue);
            
            stmt->executeUpdate();
            
            SessionManager::logAction(db, "UPDATE", "student_fees", feeId, "Updated fee record");
            
            return JsonHelper::successResponse("Fee record updated!");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_getPaymentHistory", [](const string& request) -> string {
        try {
            if (!SessionManager::hasPermission(Permissions::PAY_VIEW)) {
                return "[]";
            }
            
            string studentFeeId = JsonHelper::parseSimpleValue(request, "studentFeeId");
            
            string query = 
                "SELECT fp.*, u.full_name AS recorded_by_name "
                "FROM fee_payments fp "
                "JOIN users u ON fp.recorded_by = u.id "
                "WHERE fp.student_fee_id = ? "
                "ORDER BY fp.payment_date DESC";
            
            auto stmt = db.prepareStatement(query);
            stmt->setString(1, studentFeeId);
            auto result = stmt->executeQuery();
            
            vector<string> payments;
            while (result->next()) {
                stringstream ss;
                ss << "{";
                ss << "\"id\":" << result->getInt("id") << ",";
                ss << "\"studentFeeId\":" << result->getInt("student_fee_id") << ",";
                ss << "\"paymentDate\":\"" << result->getString("payment_date") << "\",";
                ss << "\"amount\":" << result->getDouble("amount") << ",";
                ss << "\"paymentMethod\":\"" << result->getString("payment_method") << "\",";
                ss << "\"receiptNumber\":\"" << (result->isNull("receipt_number") ? "" : result->getString("receipt_number")) << "\",";
                ss << "\"recordedBy\":" << result->getInt("recorded_by") << ",";
                ss << "\"recordedByName\":\"" << result->getString("recorded_by_name") << "\",";
                ss << "\"notes\":\"" << (result->isNull("notes") ? "" : result->getString("notes")) << "\"";
                ss << "}";
                payments.push_back(ss.str());
            }
            return JsonHelper::makeArray(payments);
        } catch (const exception& e) {
            return "[]";
        }
    });

    w.bind("cpp_recordPayment", [](const string& request) -> string {
        try {
            if (!SessionManager::hasPermission(Permissions::PAY_CREATE)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            
            string studentFeeId = JsonHelper::parseSimpleValue(request, "studentFeeId");
            string paymentDate = JsonHelper::parseSimpleValue(request, "paymentDate");
            string amount = JsonHelper::parseSimpleValue(request, "amount");
            string paymentMethod = JsonHelper::parseSimpleValue(request, "paymentMethod");
            string receiptNumber = JsonHelper::parseSimpleValue(request, "receiptNumber");
            string notes = JsonHelper::parseSimpleValue(request, "notes");
            
            // Validate required fields
            if (studentFeeId.empty() || paymentDate.empty() || amount.empty() || paymentMethod.empty()) {
                return JsonHelper::errorResponse("Student Fee ID, Payment Date, Amount, and Payment Method are required");
            }
            
            // Validate and parse numeric values
            int feeIdValue = 0;
            double amountValue = 0.0;
            
            try {
                feeIdValue = stoi(studentFeeId);
                amountValue = stod(amount);
                
                if (amountValue <= 0) {
                    return JsonHelper::errorResponse("Payment amount must be greater than zero");
                }
            } catch (const invalid_argument&) {
                return JsonHelper::errorResponse("Invalid numeric value");
            } catch (const out_of_range&) {
                return JsonHelper::errorResponse("Value out of range");
            }
            
            // Check remaining amount and prevent overpayment
            auto checkStmt = db.prepareStatement(
                "SELECT remaining_amount, status FROM student_fees WHERE id = ?"
            );
            checkStmt->setInt(1, feeIdValue);
            auto checkResult = checkStmt->executeQuery();
            
            if (!checkResult->next()) {
                return JsonHelper::errorResponse("Fee record not found");
            }
            
            double remainingAmount = checkResult->getDouble("remaining_amount");
            string currentStatus = checkResult->getString("status");
            
            if (currentStatus == "paid") {
                return JsonHelper::errorResponse("Fee is already fully paid");
            }
            
            if (amountValue > remainingAmount) {
                return JsonHelper::errorResponse("Payment amount (" + to_string(amountValue) + 
                                                 ") exceeds remaining amount (" + to_string(remainingAmount) + ")");
            }
            
            // Start transaction for atomic operation
            try {
                db.executeUpdate("START TRANSACTION");
                
                auto stmt = db.prepareStatement(
                    "INSERT INTO fee_payments (student_fee_id, payment_date, amount, "
                    "payment_method, receipt_number, recorded_by, notes) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?)"
                );
                stmt->setInt(1, feeIdValue);
                stmt->setString(2, paymentDate);
                stmt->setDouble(3, amountValue);
                stmt->setString(4, paymentMethod);
                if (receiptNumber.empty()) stmt->setNull(5); else stmt->setString(5, receiptNumber);
                stmt->setString(6, SessionManager::getSession().userId);
                if (notes.empty()) stmt->setNull(7); else stmt->setString(7, notes);
                
                stmt->executeUpdate();
                
                // Update student_fees: recalculate paid_amount, remaining_amount, and status
                auto updateStmt = db.prepareStatement(
                    "UPDATE student_fees SET "
                    "paid_amount = (SELECT COALESCE(SUM(amount), 0) FROM fee_payments WHERE student_fee_id = ?), "
                    "remaining_amount = total_fees - (SELECT COALESCE(SUM(amount), 0) FROM fee_payments WHERE student_fee_id = ?), "
                    "status = CASE "
                    "  WHEN (SELECT COALESCE(SUM(amount), 0) FROM fee_payments WHERE student_fee_id = ?) >= total_fees THEN 'paid' "
                    "  WHEN (SELECT COALESCE(SUM(amount), 0) FROM fee_payments WHERE student_fee_id = ?) > 0 THEN 'partial' "
                    "  ELSE 'unpaid' END "
                    "WHERE id = ?"
                );
                updateStmt->setInt(1, feeIdValue);
                updateStmt->setInt(2, feeIdValue);
                updateStmt->setInt(3, feeIdValue);
                updateStmt->setInt(4, feeIdValue);
                updateStmt->setInt(5, feeIdValue);
                updateStmt->executeUpdate();
                
                db.executeUpdate("COMMIT");
                
                SessionManager::logAction(db, "CREATE", "fee_payments", 
                                          to_string(db.getLastInsertId()), 
                                          "Recorded payment of " + amount + " for fee ID " + studentFeeId);
                
                return JsonHelper::successResponse("Payment recorded successfully!");
            } catch (const exception& e) {
                try {
                    db.executeUpdate("ROLLBACK");
                } catch (...) {}
                throw;
            }
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_getFeesSummary", [](const string& request) -> string {
        try {
            if (!SessionManager::hasPermission(Permissions::FEE_VIEW)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            
            string semesterId = JsonHelper::parseSimpleValue(request, "semesterId");
            
            string baseQuery = "SELECT COUNT(*) as total_students, "
                             "COALESCE(SUM(total_fees), 0) as total_fees, "
                             "COALESCE(SUM(paid_amount), 0) as total_paid, "
                             "COALESCE(SUM(remaining_amount), 0) as total_remaining, "
                             "SUM(CASE WHEN status = 'paid' THEN 1 ELSE 0 END) as paid_count, "
                             "SUM(CASE WHEN status = 'partial' THEN 1 ELSE 0 END) as partial_count, "
                             "SUM(CASE WHEN status = 'unpaid' THEN 1 ELSE 0 END) as unpaid_count "
                             "FROM student_fees ";
            
            if (!semesterId.empty()) {
                baseQuery += "WHERE semester_id = ?";
                auto stmt = db.prepareStatement(baseQuery);
                stmt->setString(1, semesterId);
                auto result = stmt->executeQuery();
                
                if (result->next()) {
                    stringstream ss;
                    ss << "{";
                    ss << "\"totalStudents\":" << result->getInt("total_students") << ",";
                    ss << "\"totalFees\":" << result->getDouble("total_fees") << ",";
                    ss << "\"totalPaid\":" << result->getDouble("total_paid") << ",";
                    ss << "\"totalRemaining\":" << result->getDouble("total_remaining") << ",";
                    ss << "\"paidCount\":" << result->getInt("paid_count") << ",";
                    ss << "\"partialCount\":" << result->getInt("partial_count") << ",";
                    ss << "\"unpaidCount\":" << result->getInt("unpaid_count");
                    ss << "}";
                    return ss.str();
                }
            } else {
                auto result = db.executeQuery(baseQuery);
                if (result->next()) {
                    stringstream ss;
                    ss << "{";
                    ss << "\"totalStudents\":" << result->getInt("total_students") << ",";
                    ss << "\"totalFees\":" << result->getDouble("total_fees") << ",";
                    ss << "\"totalPaid\":" << result->getDouble("total_paid") << ",";
                    ss << "\"totalRemaining\":" << result->getDouble("total_remaining") << ",";
                    ss << "\"paidCount\":" << result->getInt("paid_count") << ",";
                    ss << "\"partialCount\":" << result->getInt("partial_count") << ",";
                    ss << "\"unpaidCount\":" << result->getInt("unpaid_count");
                    ss << "}";
                    return ss.str();
                }
            }
            return "{}";
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    // ========================================
    // DROPDOWN DATA
    // ========================================
    w.bind("cpp_getAcademicLevels", [](const string& request) -> string {
        try {
            auto result = db.executeQuery(
                "SELECT id, name_en, year_number FROM academic_levels ORDER BY year_number"
            );
            vector<string> levels;
            while (result->next()) {
                vector<pair<string, string>> level;
                level.push_back({"id", JsonHelper::makeString(result->getString("id"))});
                level.push_back({"name", JsonHelper::makeString(result->getString("name_en"))});
                level.push_back({"year", JsonHelper::makeNumber(result->getInt("year_number"))});
                levels.push_back(JsonHelper::makeObject(level));
            }
            return JsonHelper::makeArray(levels);
        } catch (...) { return "[]"; }
    });

    w.bind("cpp_getDepartments", [](const string& request) -> string {
        try {
            auto result = db.executeQuery(
                "SELECT d.id, d.name_en, d.college_id, d.code, c.name_en as college_name "
                "FROM departments d JOIN colleges c ON d.college_id = c.id ORDER BY d.name_en"
            );
            vector<string> depts;
            while (result->next()) {
                vector<pair<string, string>> dept;
                dept.push_back({"id", JsonHelper::makeString(result->getString("id"))});
                dept.push_back({"name", JsonHelper::makeString(result->getString("name_en"))});
                dept.push_back({"collegeId", JsonHelper::makeString(result->getString("college_id"))});
                dept.push_back({"collegeName", JsonHelper::makeString(result->getString("college_name"))});
                dept.push_back({"code", JsonHelper::makeString(result->getString("code"))});
                depts.push_back(JsonHelper::makeObject(dept));
            }
            return JsonHelper::makeArray(depts);
        } catch (...) { return "[]"; }
    });

    w.bind("cpp_getColleges", [](const string& request) -> string {
        try {
            auto result = db.executeQuery("SELECT id, name_en FROM colleges ORDER BY name_en");
            vector<string> colleges;
            while (result->next()) {
                vector<pair<string, string>> col;
                col.push_back({"id", JsonHelper::makeString(result->getString("id"))});
                col.push_back({"name", JsonHelper::makeString(result->getString("name_en"))});
                colleges.push_back(JsonHelper::makeObject(col));
            }
            return JsonHelper::makeArray(colleges);
        } catch (...) { return "[]"; }
    });

    w.bind("cpp_getLectureHalls", [](const string& request) -> string {
        try {
            auto result = db.executeQuery(
                "SELECT id, name, building, floor, max_capacity, seating_desc, "
                "ac_units, fan_units, lighting_units, description "
                "FROM lecture_halls ORDER BY name"
            );
            vector<string> halls;
            while (result->next()) {
                vector<pair<string, string>> hall;
                hall.push_back({"id", JsonHelper::makeString(result->getString("id"))});
                hall.push_back({"name", JsonHelper::makeString(result->getString("name"))});
                hall.push_back({"building", JsonHelper::makeString(result->getString("building"))});
                hall.push_back({"floor", JsonHelper::makeNumber(result->getInt("floor"))});
                hall.push_back({"maxCapacity", JsonHelper::makeNumber(result->getInt("max_capacity"))});
                hall.push_back({"seatingDesc", JsonHelper::makeString(result->getString("seating_desc"))});
                hall.push_back({"acUnits", JsonHelper::makeNumber(result->getInt("ac_units"))});
                hall.push_back({"fanUnits", JsonHelper::makeNumber(result->getInt("fan_units"))});
                hall.push_back({"lightingUnits", JsonHelper::makeNumber(result->getInt("lighting_units"))});
                hall.push_back({"description", JsonHelper::makeString(result->getString("description"))});
                halls.push_back(JsonHelper::makeObject(hall));
            }
            return JsonHelper::makeArray(halls);
        } catch (...) { return "[]"; }
    });

    w.bind("cpp_getLaboratories", [](const string& request) -> string {
        try {
            auto result = db.executeQuery(
                "SELECT id, name, building, floor, max_capacity, lab_type, "
                "computers_count, seats_count, ac_units, fan_units, lighting_units, description "
                "FROM laboratories ORDER BY name"
            );
            vector<string> labs;
            while (result->next()) {
                vector<pair<string, string>> lab;
                lab.push_back({"id", JsonHelper::makeString(result->getString("id"))});
                lab.push_back({"name", JsonHelper::makeString(result->getString("name"))});
                lab.push_back({"building", JsonHelper::makeString(result->getString("building"))});
                lab.push_back({"floor", JsonHelper::makeNumber(result->getInt("floor"))});
                lab.push_back({"maxCapacity", JsonHelper::makeNumber(result->getInt("max_capacity"))});
                lab.push_back({"labType", JsonHelper::makeString(result->getString("lab_type"))});
                lab.push_back({"computersCount", JsonHelper::makeNumber(result->getInt("computers_count"))});
                lab.push_back({"seatsCount", JsonHelper::makeNumber(result->getInt("seats_count"))});
                lab.push_back({"acUnits", JsonHelper::makeNumber(result->getInt("ac_units"))});
                lab.push_back({"fanUnits", JsonHelper::makeNumber(result->getInt("fan_units"))});
                lab.push_back({"lightingUnits", JsonHelper::makeNumber(result->getInt("lighting_units"))});
                lab.push_back({"description", JsonHelper::makeString(result->getString("description"))});
                labs.push_back(JsonHelper::makeObject(lab));
            }
            return JsonHelper::makeArray(labs);
        } catch (...) { return "[]"; }
    });

    w.bind("cpp_getSemesters", [](const string& request) -> string {
        try {
            auto result = db.executeQuery(
                "SELECT id, name_en, academic_year, semester_number, is_current "
                "FROM semesters ORDER BY academic_year DESC, semester_number"
            );
            vector<string> semesters;
            while (result->next()) {
                vector<pair<string, string>> sem;
                sem.push_back({"id", JsonHelper::makeString(result->getString("id"))});
                sem.push_back({"name", JsonHelper::makeString(result->getString("name_en"))});
                sem.push_back({"academicYear", JsonHelper::makeString(result->getString("academic_year"))});
                sem.push_back({"semesterNumber", JsonHelper::makeNumber(result->getInt("semester_number"))});
                sem.push_back({"isCurrent", JsonHelper::makeBool(result->getBoolean("is_current"))});
                semesters.push_back(JsonHelper::makeObject(sem));
            }
            return JsonHelper::makeArray(semesters);
        } catch (...) { return "[]"; }
    });

    w.bind("cpp_getCourseTypes", [](const string& request) -> string {
        try {
            auto result = db.executeQuery(
                "SELECT id, name, max_marks, year_work_max, lab_max, practical_max, written_exam_max "
                "FROM course_types ORDER BY name"
            );
            vector<string> types;
            while (result->next()) {
                vector<pair<string, string>> ct;
                ct.push_back({"id", JsonHelper::makeString(result->getString("id"))});
                ct.push_back({"name", JsonHelper::makeString(result->getString("name"))});
                ct.push_back({"maxMarks", JsonHelper::makeNumber(result->getInt("max_marks"))});
                ct.push_back({"yearWorkMax", JsonHelper::makeNumber(result->getInt("year_work_max"))});
                ct.push_back({"labMax", JsonHelper::makeNumber(result->getInt("lab_max"))});
                ct.push_back({"practicalMax", JsonHelper::makeNumber(result->getInt("practical_max"))});
                ct.push_back({"writtenMax", JsonHelper::makeNumber(result->getInt("written_exam_max"))});
                types.push_back(JsonHelper::makeObject(ct));
            }
            return JsonHelper::makeArray(types);
        } catch (...) { return "[]"; }
    });

    w.bind("cpp_getStudentGrades", [](const string& request) -> string {
        try {
            string studentId = JsonHelper::parseSimpleValue(request, "studentId");
            studentId.erase(remove(studentId.begin(), studentId.end(), '['), studentId.end());
            studentId.erase(remove(studentId.begin(), studentId.end(), ']'), studentId.end());
            studentId.erase(remove(studentId.begin(), studentId.end(), '"'), studentId.end());

            if (!SessionManager::isLoggedIn()) {
                return "[]";
            }
            const UserSession& session = SessionManager::getSession();
            if (session.isStudent() && !SessionManager::isOwnStudentId(studentId)) {
                return "[]";
            }
            if (session.isProfessor()) {
                return "[]";
            }
            if (!session.isStudent() && !SessionManager::canViewAllGrades()) {
                return "[]";
            }

            auto recalcStmt = db.prepareStatement(
                "SELECT id FROM registrations WHERE student_id = ?"
            );
            recalcStmt->setString(1, studentId);
            auto recalcRes = recalcStmt->executeQuery();
            while (recalcRes->next()) {
                Grade::calculateGrade(db, recalcRes->getString("id"));
            }

            auto stmt = db.prepareStatement(
                "SELECT r.id AS registration_id, c.code AS course_code, c.name_en AS course_name, "
                "ct.name AS course_type, c.max_marks, c.credit_hours, "
                "sem.name_en AS semester_name, "
                "COALESCE(g.assignment1, 0) AS assignment1, COALESCE(g.assignment2, 0) AS assignment2, "
                "COALESCE(g.year_work, 0) AS year_work, COALESCE(g.final_exam, 0) AS final_exam, "
                "COALESCE(g.total_marks, 0) AS total_marks, COALESCE(g.percentage, 0) AS percentage, "
                "COALESCE(g.gpa, 0) AS gpa, COALESCE(g.evaluation, 'Fail') AS evaluation, "
                "COALESCE(g.letter_grade, 'F') AS letter_grade "
                "FROM registrations r "
                "JOIN courses c ON r.course_id = c.id "
                "JOIN course_types ct ON c.course_type_id = ct.id "
                "JOIN semesters sem ON r.semester_id = sem.id "
                "LEFT JOIN grades g ON r.id = g.registration_id "
                "WHERE r.student_id = ? "
                "ORDER BY sem.academic_year DESC, sem.semester_number"
            );
            stmt->setString(1, studentId);
            auto result = stmt->executeQuery();

            vector<string> students;
            
            while (result->next()) {
                double assignment1 = result->getDouble("assignment1");
                double assignment2 = result->getDouble("assignment2");
                double yearWork = result->getDouble("year_work");
                double finalExam = result->getDouble("final_exam");
                
                // Check if any grades have been entered
                bool gradesEntered = (assignment1 + assignment2 + yearWork + finalExam) > 0;
                
                vector<pair<string, string>> student;
                student.push_back({"registrationId", JsonHelper::makeString(result->getString("registration_id"))});
                student.push_back({"studentId", JsonHelper::makeString(result->getString("student_id"))});
                student.push_back({"studentName", JsonHelper::makeString(result->getString("student_name"))});
                student.push_back({"assignment1", JsonHelper::makeNumber(assignment1)});
                student.push_back({"assignment2", JsonHelper::makeNumber(assignment2)});
                student.push_back({"yearWork", JsonHelper::makeNumber(yearWork)});
                student.push_back({"finalExam", JsonHelper::makeNumber(finalExam)});
                student.push_back({"total", JsonHelper::makeNumber(result->getDouble("total_marks"))});
                student.push_back({"percentage", JsonHelper::makeNumber(result->getDouble("percentage"))});
                student.push_back({"gpa", JsonHelper::makeNumber(result->getDouble("gpa"))});
                student.push_back({"evaluation", JsonHelper::makeString(result->getString("evaluation"))});
                student.push_back({"letterGrade", JsonHelper::makeString(result->getString("letter_grade"))});
                student.push_back({"gradesEntered", gradesEntered ? "true" : "false"});
                students.push_back(JsonHelper::makeObject(student));
            }
            return JsonHelper::makeArray(students);
        } catch (const exception& e) {
            return "[]";
        }
    });

    w.bind("cpp_getStudentCGPA", [](const string& request) -> string {
        try {
            string studentId = JsonHelper::parseSimpleValue(request, "studentId");
            studentId.erase(remove(studentId.begin(), studentId.end(), '['), studentId.end());
            studentId.erase(remove(studentId.begin(), studentId.end(), ']'), studentId.end());
            studentId.erase(remove(studentId.begin(), studentId.end(), '"'), studentId.end());

            if (!SessionManager::isLoggedIn()) {
                return "{\"cgpa\": 0.00}";
            }
            const UserSession& session = SessionManager::getSession();
            if (session.isStudent() && !SessionManager::isOwnStudentId(studentId)) {
                return "{\"cgpa\": 0.00}";
            }
            if (session.isProfessor()) {
                return "{\"cgpa\": 0.00}";
            }
            if (!session.isStudent() && !SessionManager::canViewAllGrades()) {
                return "{\"cgpa\": 0.00}";
            }

            auto recalcStmt = db.prepareStatement(
                "SELECT id FROM registrations WHERE student_id = ?"
            );
            recalcStmt->setString(1, studentId);
            auto recalcRes = recalcStmt->executeQuery();
            while (recalcRes->next()) {
                Grade::calculateGrade(db, recalcRes->getString("id"));
            }

            double cgpa = Grade::calculateStudentCGPA(db, studentId);
            vector<pair<string, string>> result;
            result.push_back({"cgpa", JsonHelper::makeNumber(cgpa)});
            return JsonHelper::makeObject(result);
        } catch (const exception& e) {
            return "{\"cgpa\": 0.00}";
        }
    });

    // ========================================
    // Student Profile API
    // ========================================
    w.bind("cpp_getStudentProfile", [](const string& request) -> string {
        try {
            string studentId = request;
            studentId.erase(remove(studentId.begin(), studentId.end(), '['), studentId.end());
            studentId.erase(remove(studentId.begin(), studentId.end(), ']'), studentId.end());
            studentId.erase(remove(studentId.begin(), studentId.end(), '"'), studentId.end());

            if (!SessionManager::isLoggedIn()) {
                return "{}";
            }
            const UserSession& session = SessionManager::getSession();
            // Students can only view their own profile
            if (session.isStudent() && !SessionManager::isOwnStudentId(studentId)) {
                return "{}";
            }

            auto stmt = db.prepareStatement(
                "SELECT s.id, s.first_name, s.last_name, s.date_of_birth, s.gender, "
                "s.email, s.phone, s.enrollment_date, s.status, "
                "al.id AS level_id, al.name_en AS level_name, al.year_number, "
                "d.id AS dept_id, d.name_en AS dept_name, d.code AS dept_code, "
                "c.id AS college_id, c.name_en AS college_name "
                "FROM students s "
                "LEFT JOIN academic_levels al ON s.academic_level_id = al.id "
                "LEFT JOIN departments d ON s.department_id = d.id "
                "LEFT JOIN colleges c ON d.college_id = c.id "
                "WHERE s.id = ?"
            );
            stmt->setString(1, studentId);
            auto result = stmt->executeQuery();

            if (!result->next()) {
                return "{}";
            }

            // Get count of registered courses
            auto courseStmt = db.prepareStatement(
                "SELECT COUNT(*) as total_courses, "
                "SUM(CASE WHEN sem.is_current = TRUE THEN 1 ELSE 0 END) as current_courses "
                "FROM registrations r "
                "LEFT JOIN semesters sem ON r.semester_id = sem.id "
                "WHERE r.student_id = ?"
            );
            courseStmt->setString(1, studentId);
            auto courseResult = courseStmt->executeQuery();
            int totalCourses = 0;
            int currentCourses = 0;
            if (courseResult->next()) {
                totalCourses = courseResult->getInt("total_courses");
                currentCourses = courseResult->getInt("current_courses");
            }

            vector<pair<string, string>> profile;
            profile.push_back({"id", JsonHelper::makeString(result->getString("id"))});
            profile.push_back({"firstName", JsonHelper::makeString(result->getString("first_name"))});
            profile.push_back({"lastName", JsonHelper::makeString(result->getString("last_name"))});
            profile.push_back({"fullName", JsonHelper::makeString(result->getString("first_name") + " " + result->getString("last_name"))});
            profile.push_back({"dateOfBirth", JsonHelper::makeString(result->getString("date_of_birth"))});
            profile.push_back({"gender", JsonHelper::makeString(result->getString("gender"))});
            profile.push_back({"email", JsonHelper::makeString(result->getString("email"))});
            profile.push_back({"phone", JsonHelper::makeString(result->getString("phone"))});
            profile.push_back({"enrollmentDate", JsonHelper::makeString(result->getString("enrollment_date"))});
            profile.push_back({"status", JsonHelper::makeString(result->getString("status"))});
            profile.push_back({"levelId", JsonHelper::makeString(result->getString("level_id"))});
            profile.push_back({"levelName", JsonHelper::makeString(result->getString("level_name"))});
            profile.push_back({"yearNumber", JsonHelper::makeNumber(result->getInt("year_number"))});
            profile.push_back({"departmentId", JsonHelper::makeString(result->getString("dept_id"))});
            profile.push_back({"departmentName", JsonHelper::makeString(result->getString("dept_name"))});
            profile.push_back({"departmentCode", JsonHelper::makeString(result->getString("dept_code"))});
            profile.push_back({"collegeId", JsonHelper::makeString(result->getString("college_id"))});
            profile.push_back({"collegeName", JsonHelper::makeString(result->getString("college_name"))});
            profile.push_back({"totalCourses", JsonHelper::makeNumber(totalCourses)});
            profile.push_back({"currentCourses", JsonHelper::makeNumber(currentCourses)});

            return JsonHelper::makeObject(profile);
        } catch (const exception& e) {
            cerr << "[Profile] Error: " << e.what() << endl;
            return "{}";
        }
    });

    w.bind("cpp_deleteRegistration", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::REG_DELETE)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            string id = request;
            id.erase(remove(id.begin(), id.end(), '['), id.end());
            id.erase(remove(id.begin(), id.end(), ']'), id.end());
            id.erase(remove(id.begin(), id.end(), '"'), id.end());

            auto stmt = db.prepareStatement("DELETE FROM registrations WHERE id = ?");
            stmt->setString(1, id);
            int rows = stmt->executeUpdate();

            if (rows > 0) {
                SessionManager::logAction(db, "DELETE", "registrations", id, "Deleted registration");
                return JsonHelper::successResponse("Registration deleted!");
            }
            return JsonHelper::errorResponse("Registration not found");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_updateRegistrationStatus", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::REG_EDIT)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            string id = JsonHelper::parseSimpleValue(request, "id");
            string status = JsonHelper::parseSimpleValue(request, "status");

            auto stmt = db.prepareStatement("UPDATE registrations SET status = ? WHERE id = ?");
            stmt->setString(1, status);
            stmt->setString(2, id);
            stmt->executeUpdate();

            SessionManager::logAction(db, "UPDATE", "registrations", id, "Status = " + status);
            return JsonHelper::successResponse("Status updated!");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_addCourse", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::CRS_CREATE)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            string id = JsonHelper::parseSimpleValue(request, "id");
            string code = JsonHelper::parseSimpleValue(request, "code");
            string nameEn = JsonHelper::parseSimpleValue(request, "nameEn");
            string nameAr = JsonHelper::parseSimpleValue(request, "nameAr");
            string description = JsonHelper::parseSimpleValue(request, "description");
            string departmentId = JsonHelper::parseSimpleValue(request, "departmentId");
            string academicLevelId = JsonHelper::parseSimpleValue(request, "academicLevelId");
            string courseTypeId = JsonHelper::parseSimpleValue(request, "courseTypeId");
            int semesterNumber = stoi(JsonHelper::parseSimpleValue(request, "semesterNumber"));
            int creditHours = stoi(JsonHelper::parseSimpleValue(request, "creditHours"));
            string lectureHoursStr = JsonHelper::parseSimpleValue(request, "lectureHours");
            string tutorialHoursStr = JsonHelper::parseSimpleValue(request, "tutorialHours");
            string labHoursStr = JsonHelper::parseSimpleValue(request, "labHours");
            string practicalHoursStr = JsonHelper::parseSimpleValue(request, "practicalHours");
            string maxMarksStr = JsonHelper::parseSimpleValue(request, "maxMarks");
            string yearWorkStr = JsonHelper::parseSimpleValue(request, "yearWorkMarks");
            string assignment1Str = JsonHelper::parseSimpleValue(request, "assignment1Marks");
            string assignment2Str = JsonHelper::parseSimpleValue(request, "assignment2Marks");
            string finalExamStr = JsonHelper::parseSimpleValue(request, "finalExamMarks");

            int lectureHours = lectureHoursStr.empty() ? 0 : stoi(lectureHoursStr);
            int tutorialHours = tutorialHoursStr.empty() ? 0 : stoi(tutorialHoursStr);
            int labHours = labHoursStr.empty() ? 0 : stoi(labHoursStr);
            int practicalHours = practicalHoursStr.empty() ? 0 : stoi(practicalHoursStr);
            int maxMarks = maxMarksStr.empty() ? 0 : stoi(maxMarksStr);
            int yearWorkMarks = yearWorkStr.empty() ? 0 : stoi(yearWorkStr);
            int assignment1Marks = assignment1Str.empty() ? 0 : stoi(assignment1Str);
            int assignment2Marks = assignment2Str.empty() ? 0 : stoi(assignment2Str);
            int finalExamMarks = finalExamStr.empty() ? 0 : stoi(finalExamStr);

            int derivedMax = yearWorkMarks + assignment1Marks + assignment2Marks + finalExamMarks;
            if (maxMarks <= 0 && derivedMax > 0) {
                maxMarks = derivedMax;
            }
            if (maxMarks <= 0) {
                return JsonHelper::errorResponse("Max marks must be > 0");
            }
            if (derivedMax > 0 && maxMarks != derivedMax) {
                return JsonHelper::errorResponse("Component marks must sum to max marks");
            }
            if (semesterNumber < 1 || semesterNumber > 2) {
                return JsonHelper::errorResponse("Semester number must be 1 or 2");
            }
            if (creditHours < 0 || creditHours > 6) {
                return JsonHelper::errorResponse("Credit hours must be between 0 and 6");
            }

            // Check if course ID exists
            auto idCheckStmt = db.prepareStatement("SELECT COUNT(*) as cnt FROM courses WHERE id = ?");
            idCheckStmt->setString(1, id);
            auto idCheckResult = idCheckStmt->executeQuery();
            if (idCheckResult->next() && idCheckResult->getInt("cnt") > 0) {
                return JsonHelper::errorResponse("Course ID already exists!");
            }

            // Check if course code exists
            auto checkStmt = db.prepareStatement("SELECT COUNT(*) as cnt FROM courses WHERE code = ?");
            checkStmt->setString(1, code);
            auto checkResult = checkStmt->executeQuery();
            if (checkResult->next() && checkResult->getInt("cnt") > 0) {
                return JsonHelper::errorResponse("Course code already exists!");
            }

            auto stmt = db.prepareStatement(
                "INSERT INTO courses (id, code, name_en, name_ar, description, department_id, academic_level_id, "
                "semester_number, course_type_id, credit_hours, lecture_hours, tutorial_hours, lab_hours, practical_hours, "
                "max_marks, year_work_marks, assignment1_marks, assignment2_marks, final_exam_marks) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
            );
            stmt->setString(1, id);
            stmt->setString(2, code);
            stmt->setString(3, nameEn);
            stmt->setString(4, nameAr);
            stmt->setString(5, description);
            stmt->setString(6, departmentId);
            stmt->setString(7, academicLevelId);
            stmt->setInt(8, semesterNumber);
            stmt->setString(9, courseTypeId);
            stmt->setInt(10, creditHours);
            stmt->setInt(11, lectureHours);
            stmt->setInt(12, tutorialHours);
            stmt->setInt(13, labHours);
            stmt->setInt(14, practicalHours);
            stmt->setInt(15, maxMarks);
            stmt->setInt(16, yearWorkMarks);
            stmt->setInt(17, assignment1Marks);
            stmt->setInt(18, assignment2Marks);
            stmt->setInt(19, finalExamMarks);
            stmt->executeUpdate();

            SessionManager::logAction(db, "CREATE", "courses", id, "Added course " + code);
            return JsonHelper::successResponse("Course added successfully!");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_updateCourse", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::CRS_EDIT)) {
                return JsonHelper::errorResponse("Permission denied");
            }

            string id = JsonHelper::parseSimpleValue(request, "id");
            string code = JsonHelper::parseSimpleValue(request, "code");
            string nameEn = JsonHelper::parseSimpleValue(request, "nameEn");
            string nameAr = JsonHelper::parseSimpleValue(request, "nameAr");
            string description = JsonHelper::parseSimpleValue(request, "description");
            string departmentId = JsonHelper::parseSimpleValue(request, "departmentId");
            string academicLevelId = JsonHelper::parseSimpleValue(request, "academicLevelId");
            string courseTypeId = JsonHelper::parseSimpleValue(request, "courseTypeId");
            int semesterNumber = stoi(JsonHelper::parseSimpleValue(request, "semesterNumber"));
            int creditHours = stoi(JsonHelper::parseSimpleValue(request, "creditHours"));
            string lectureHoursStr = JsonHelper::parseSimpleValue(request, "lectureHours");
            string tutorialHoursStr = JsonHelper::parseSimpleValue(request, "tutorialHours");
            string labHoursStr = JsonHelper::parseSimpleValue(request, "labHours");
            string practicalHoursStr = JsonHelper::parseSimpleValue(request, "practicalHours");
            string maxMarksStr = JsonHelper::parseSimpleValue(request, "maxMarks");
            string yearWorkStr = JsonHelper::parseSimpleValue(request, "yearWorkMarks");
            string assignment1Str = JsonHelper::parseSimpleValue(request, "assignment1Marks");
            string assignment2Str = JsonHelper::parseSimpleValue(request, "assignment2Marks");
            string finalExamStr = JsonHelper::parseSimpleValue(request, "finalExamMarks");

            int lectureHours = lectureHoursStr.empty() ? 0 : stoi(lectureHoursStr);
            int tutorialHours = tutorialHoursStr.empty() ? 0 : stoi(tutorialHoursStr);
            int labHours = labHoursStr.empty() ? 0 : stoi(labHoursStr);
            int practicalHours = practicalHoursStr.empty() ? 0 : stoi(practicalHoursStr);
            int maxMarks = maxMarksStr.empty() ? 0 : stoi(maxMarksStr);
            int yearWorkMarks = yearWorkStr.empty() ? 0 : stoi(yearWorkStr);
            int assignment1Marks = assignment1Str.empty() ? 0 : stoi(assignment1Str);
            int assignment2Marks = assignment2Str.empty() ? 0 : stoi(assignment2Str);
            int finalExamMarks = finalExamStr.empty() ? 0 : stoi(finalExamStr);

            int derivedMax = yearWorkMarks + assignment1Marks + assignment2Marks + finalExamMarks;
            if (maxMarks <= 0 && derivedMax > 0) {
                maxMarks = derivedMax;
            }
            if (maxMarks <= 0) {
                return JsonHelper::errorResponse("Max marks must be > 0");
            }
            if (derivedMax > 0 && maxMarks != derivedMax) {
                return JsonHelper::errorResponse("Component marks must sum to max marks");
            }
            if (semesterNumber < 1 || semesterNumber > 2) {
                return JsonHelper::errorResponse("Semester number must be 1 or 2");
            }
            if (creditHours < 0 || creditHours > 6) {
                return JsonHelper::errorResponse("Credit hours must be between 0 and 6");
            }

            auto existsStmt = db.prepareStatement("SELECT COUNT(*) as cnt FROM courses WHERE id = ?");
            existsStmt->setString(1, id);
            auto existsResult = existsStmt->executeQuery();
            if (!existsResult->next() || existsResult->getInt("cnt") == 0) {
                return JsonHelper::errorResponse("Course not found");
            }

            auto codeCheckStmt = db.prepareStatement("SELECT COUNT(*) as cnt FROM courses WHERE code = ? AND id <> ?");
            codeCheckStmt->setString(1, code);
            codeCheckStmt->setString(2, id);
            auto codeCheckResult = codeCheckStmt->executeQuery();
            if (codeCheckResult->next() && codeCheckResult->getInt("cnt") > 0) {
                return JsonHelper::errorResponse("Course code already exists!");
            }

            auto stmt = db.prepareStatement(
                "UPDATE courses SET code = ?, name_en = ?, name_ar = ?, description = ?, department_id = ?, "
                "academic_level_id = ?, semester_number = ?, course_type_id = ?, credit_hours = ?, "
                "lecture_hours = ?, tutorial_hours = ?, lab_hours = ?, practical_hours = ?, max_marks = ?, "
                "year_work_marks = ?, assignment1_marks = ?, assignment2_marks = ?, final_exam_marks = ? WHERE id = ?"
            );
            stmt->setString(1, code);
            stmt->setString(2, nameEn);
            stmt->setString(3, nameAr);
            stmt->setString(4, description);
            stmt->setString(5, departmentId);
            stmt->setString(6, academicLevelId);
            stmt->setInt(7, semesterNumber);
            stmt->setString(8, courseTypeId);
            stmt->setInt(9, creditHours);
            stmt->setInt(10, lectureHours);
            stmt->setInt(11, tutorialHours);
            stmt->setInt(12, labHours);
            stmt->setInt(13, practicalHours);
            stmt->setInt(14, maxMarks);
            stmt->setInt(15, yearWorkMarks);
            stmt->setInt(16, assignment1Marks);
            stmt->setInt(17, assignment2Marks);
            stmt->setInt(18, finalExamMarks);
            stmt->setString(19, id);
            stmt->executeUpdate();

            auto regStmt = db.prepareStatement("SELECT id FROM registrations WHERE course_id = ?");
            regStmt->setString(1, id);
            auto regRes = regStmt->executeQuery();
            while (regRes->next()) {
                Grade::calculateGrade(db, regRes->getString("id"));
            }

            SessionManager::logAction(db, "UPDATE", "courses", id, "Updated course " + code);
            return JsonHelper::successResponse("Course updated successfully!");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_deleteCourse", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::CRS_DELETE)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            string id = request;
            id.erase(remove(id.begin(), id.end(), '['), id.end());
            id.erase(remove(id.begin(), id.end(), ']'), id.end());
            id.erase(remove(id.begin(), id.end(), '"'), id.end());

            auto stmt = db.prepareStatement("DELETE FROM courses WHERE id = ?");
            stmt->setString(1, id);
            int rows = stmt->executeUpdate();

            if (rows > 0) {
                SessionManager::logAction(db, "DELETE", "courses", id, "Deleted course");
                return JsonHelper::successResponse("Course deleted!");
            }
            return JsonHelper::errorResponse("Course not found");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    // ========================================
    // ROLES & PERMISSIONS (Super Admin)
    // ========================================
    w.bind("cpp_getRoles", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::USER_MANAGE)) {
                return "[]";
            }
            auto result = db.executeQuery(
                "SELECT r.id, r.name_en, r.description, "
                "COUNT(u.id) as user_count "
                "FROM roles r "
                "LEFT JOIN users u ON r.id = u.role_id "
                "GROUP BY r.id, r.name_en, r.description "
                "ORDER BY r.name_en"
            );
            vector<string> roles;
            while (result->next()) {
                vector<pair<string, string>> role;
                role.push_back({"id", JsonHelper::makeString(result->getString("id"))});
                role.push_back({"nameEn", JsonHelper::makeString(result->getString("name_en"))});
                role.push_back({"nameAr", JsonHelper::makeString("")});
                role.push_back({"description", JsonHelper::makeString(result->getString("description"))});
                role.push_back({"userCount", JsonHelper::makeNumber(result->getInt("user_count"))});
                roles.push_back(JsonHelper::makeObject(role));
            }
            return JsonHelper::makeArray(roles);
        } catch (...) {
            return "[]";
        }
    });

    w.bind("cpp_getPermissions", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::USER_MANAGE)) {
                return "[]";
            }
            auto result = db.executeQuery(
                "SELECT id, name, description FROM permissions ORDER BY name"
            );
            vector<string> perms;
            while (result->next()) {
                vector<pair<string, string>> perm;
                perm.push_back({"id", JsonHelper::makeString(result->getString("id"))});
                perm.push_back({"name", JsonHelper::makeString(result->getString("name"))});
                perm.push_back({"description", JsonHelper::makeString(result->getString("description"))});
                perms.push_back(JsonHelper::makeObject(perm));
            }
            return JsonHelper::makeArray(perms);
        } catch (...) {
            return "[]";
        }
    });

    w.bind("cpp_getRolePermissions", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::USER_MANAGE)) {
                return "[]";
            }
            string roleId = JsonHelper::parseSimpleValue(request, "roleId");
            auto stmt = db.prepareStatement(
                "SELECT permission_id FROM role_permissions WHERE role_id = ?"
            );
            stmt->setString(1, roleId);
            auto result = stmt->executeQuery();

            vector<string> perms;
            while (result->next()) {
                perms.push_back(JsonHelper::makeString(result->getString("permission_id")));
            }
            return JsonHelper::makeArray(perms);
        } catch (...) {
            return "[]";
        }
    });

    w.bind("cpp_updateRolePermissions", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::USER_MANAGE)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            string roleId = JsonHelper::parseSimpleValue(request, "roleId");
            string permissionsCsv = JsonHelper::parseSimpleValue(request, "permissions");

            vector<string> permissionIds;
            string token;
            stringstream ss(permissionsCsv);
            while (getline(ss, token, ',')) {
                if (!token.empty()) {
                    permissionIds.push_back(token);
                }
            }

            // Check for exclusive permissions (only Super Admin can have them)
            if (roleId != "ROLE_SUPERADMIN") {
                for (const auto& permId : permissionIds) {
                    auto permCheckStmt = db.prepareStatement(
                        "SELECT name FROM permissions WHERE id = ?"
                    );
                    permCheckStmt->setString(1, permId);
                    auto permResult = permCheckStmt->executeQuery();
                    if (permResult->next()) {
                        string permName = permResult->getString("name");
                        if (ExclusivePermissions::isExclusive(permName)) {
                            return JsonHelper::errorResponse(
                                "Cannot assign exclusive permission: " + permName + 
                                " (reserved for Super Admin only)"
                            );
                        }
                    }
                }
            }
            
            db.executeUpdate("START TRANSACTION");
            auto clearStmt = db.prepareStatement("DELETE FROM role_permissions WHERE role_id = ?");
            clearStmt->setString(1, roleId);
            clearStmt->executeUpdate();

            for (const auto& permId : permissionIds) {
                auto insertStmt = db.prepareStatement(
                    "INSERT INTO role_permissions (role_id, permission_id) VALUES (?, ?)"
                );
                insertStmt->setString(1, roleId);
                insertStmt->setString(2, permId);
                insertStmt->executeUpdate();
            }
            db.executeUpdate("COMMIT");

            SessionManager::logAction(db, "UPDATE", "role_permissions", roleId,
                                      "Updated role permissions");
            return JsonHelper::successResponse("Role permissions updated!");
        } catch (const exception& e) {
            try { db.executeUpdate("ROLLBACK"); } catch (...) {}
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_createRole", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::ROLE_MANAGE)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            string id = JsonHelper::parseSimpleValue(request, "id");
            string name = JsonHelper::parseSimpleValue(request, "nameEn");
            string description = JsonHelper::parseSimpleValue(request, "description");

            if (id.empty() || name.empty()) {
                return JsonHelper::errorResponse("Role ID and name are required");
            }

            // Check if role ID already exists
            auto checkStmt = db.prepareStatement("SELECT COUNT(*) as cnt FROM roles WHERE id = ?");
            checkStmt->setString(1, id);
            auto checkResult = checkStmt->executeQuery();
            if (checkResult->next() && checkResult->getInt("cnt") > 0) {
                return JsonHelper::errorResponse("Role ID already exists!");
            }

            auto stmt = db.prepareStatement(
                "INSERT INTO roles (id, name_en, description) VALUES (?, ?, ?)"
            );
            stmt->setString(1, id);
            stmt->setString(2, name);
            if (description.empty()) stmt->setNull(3); else stmt->setString(3, description);
            stmt->executeUpdate();

            SessionManager::logAction(db, "CREATE", "roles", id, "Created role: " + name);
            return JsonHelper::successResponse("Role created successfully!");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_updateRole", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::ROLE_MANAGE)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            string id = JsonHelper::parseSimpleValue(request, "id");
            string name = JsonHelper::parseSimpleValue(request, "nameEn");
            string description = JsonHelper::parseSimpleValue(request, "description");

            if (id.empty() || name.empty()) {
                return JsonHelper::errorResponse("Role ID and name are required");
            }
            
            // Prevent modification of system roles
            if (id == "ROLE_SUPERADMIN" || id == "ROLE_STUDENT_AFFAIRS" || 
                id == "ROLE_PROFESSOR" || id == "ROLE_STUDENT") {
                return JsonHelper::errorResponse("Cannot modify system roles");
            }

            // Check if role exists
            auto checkStmt = db.prepareStatement("SELECT COUNT(*) as cnt FROM roles WHERE id = ?");
            checkStmt->setString(1, id);
            auto checkResult = checkStmt->executeQuery();
            if (!checkResult->next() || checkResult->getInt("cnt") == 0) {
                return JsonHelper::errorResponse("Role not found");
            }

            auto stmt = db.prepareStatement(
                "UPDATE roles SET name_en = ?, description = ? WHERE id = ?"
            );
            stmt->setString(1, name);
            if (description.empty()) stmt->setNull(2); else stmt->setString(2, description);
            stmt->setString(3, id);
            stmt->executeUpdate();

            SessionManager::logAction(db, "UPDATE", "roles", id, "Updated role: " + name);
            return JsonHelper::successResponse("Role updated successfully!");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_deleteRole", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::ROLE_MANAGE)) {
                return JsonHelper::errorResponse("Permission denied");
            }
            string id = request;
            id.erase(remove(id.begin(), id.end(), '['), id.end());
            id.erase(remove(id.begin(), id.end(), ']'), id.end());
            id.erase(remove(id.begin(), id.end(), '"'), id.end());

            // Protect system roles
            if (id == "ROLE_SUPERADMIN" || id == "ROLE_STUDENT_AFFAIRS" || 
                id == "ROLE_PROFESSOR" || id == "ROLE_STUDENT") {
                return JsonHelper::errorResponse("Cannot delete system roles!");
            }

            // Check if any users are using this role
            auto userCheckStmt = db.prepareStatement("SELECT COUNT(*) as cnt FROM users WHERE role_id = ?");
            userCheckStmt->setString(1, id);
            auto userCheckResult = userCheckStmt->executeQuery();
            if (userCheckResult->next() && userCheckResult->getInt("cnt") > 0) {
                return JsonHelper::errorResponse("Cannot delete role: Users are assigned to this role");
            }

            auto stmt = db.prepareStatement("DELETE FROM roles WHERE id = ?");
            stmt->setString(1, id);
            int rows = stmt->executeUpdate();

            if (rows > 0) {
                SessionManager::logAction(db, "DELETE", "roles", id, "Deleted role");
                return JsonHelper::successResponse("Role deleted successfully!");
            }
            return JsonHelper::errorResponse("Role not found");
        } catch (const exception& e) {
            return JsonHelper::errorResponse(e.what());
        }
    });

    w.bind("cpp_getRoleUsers", [](const string& request) -> string {
        try {
            if (!SessionManager::isLoggedIn() || !SessionManager::hasPermission(Permissions::USER_MANAGE)) {
                return "[]";
            }
            
            string roleId = JsonHelper::parseSimpleValue(request, "roleId");
            string query = 
                "SELECT u.id, u.username, u.full_name, u.email, u.phone, "
                "u.is_active, u.last_login, u.created_at "
                "FROM users u ";
            
            if (!roleId.empty()) {
                query += "WHERE u.role_id = ? ";
            }
            query += "ORDER BY u.full_name";
            
            auto stmt = db.prepareStatement(query);
            if (!roleId.empty()) {
                stmt->setString(1, roleId);
            }
            auto result = stmt->executeQuery();
            
            vector<string> users;
            while (result->next()) {
                stringstream ss;
                ss << "{";
                ss << "\"id\":" << result->getInt("id") << ",";
                ss << "\"username\":\"" << result->getString("username") << "\",";
                ss << "\"fullName\":\"" << result->getString("full_name") << "\",";
                ss << "\"email\":\"" << (result->isNull("email") ? "" : result->getString("email")) << "\",";
                ss << "\"phone\":\"" << (result->isNull("phone") ? "" : result->getString("phone")) << "\",";
                ss << "\"isActive\":" << (result->getBoolean("is_active") ? "true" : "false") << ",";
                ss << "\"lastLogin\":\"" << (result->isNull("last_login") ? "" : result->getString("last_login")) << "\",";
                ss << "\"createdAt\":\"" << result->getString("created_at") << "\"";
                ss << "}";
                users.push_back(ss.str());
            }
            return JsonHelper::makeArray(users);
        } catch (const exception& e) {
            cerr << "[RoleUsers] Error: " << e.what() << endl;
            return "[]";
        }
    });

    // ========================================
    // LOAD APPLICATION
    // ========================================
    std::filesystem::path webPath = std::filesystem::current_path() / "web" / "index.html";
    cout << "[Main] Loading: " << webPath.generic_string() << endl;
    w.navigate(toFileUrl(webPath));

    cout << "[Main] Application started." << endl;
    cout << "========================================" << endl;

    w.run();

    cout << "[Main] Application closed." << endl;
    return 0;
}
