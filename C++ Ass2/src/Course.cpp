/*
 * ============================================
 * COURSE.CPP - Course Class Implementation
 * ============================================
 */

#include "../include/Course.h"
#include <sstream>

using namespace std;

// ========================================
// CONSTRUCTORS
// ========================================

Course::Course()
    : id(""), code(""), nameEn(""), nameAr(""), description(""),
      departmentId(""), academicLevelId(""), semesterNumber(1),
      courseTypeId(""), creditHours(0), lectureHours(0), tutorialHours(0),
      labHours(0), practicalHours(0), maxMarks(0), assignment1Marks(0),
      assignment2Marks(0), yearWorkMarks(0), finalExamMarks(0) {
}

Course::Course(const string& id, const string& code, const string& nameEn,
               const string& nameAr, const string& description,
               const string& departmentId, const string& academicLevelId,
               int semesterNumber, const string& courseTypeId, int creditHours,
               int lectureHours, int tutorialHours, int labHours, int practicalHours,
               int maxMarks, int assignment1Marks, int assignment2Marks, int yearWorkMarks,
               int finalExamMarks)
    : id(id), code(code), nameEn(nameEn), nameAr(nameAr), description(description),
      departmentId(departmentId), academicLevelId(academicLevelId),
      semesterNumber(semesterNumber), courseTypeId(courseTypeId),
      creditHours(creditHours), lectureHours(lectureHours),
      tutorialHours(tutorialHours), labHours(labHours),
      practicalHours(practicalHours), maxMarks(maxMarks),
      assignment1Marks(assignment1Marks), assignment2Marks(assignment2Marks),
      yearWorkMarks(yearWorkMarks), finalExamMarks(finalExamMarks) {
}

// ========================================
// GETTERS
// ========================================

string Course::getId() const { return id; }
string Course::getCode() const { return code; }
string Course::getNameEn() const { return nameEn; }
string Course::getNameAr() const { return nameAr; }
string Course::getDescription() const { return description; }
string Course::getDepartmentId() const { return departmentId; }
string Course::getAcademicLevelId() const { return academicLevelId; }
int Course::getSemesterNumber() const { return semesterNumber; }
string Course::getCourseTypeId() const { return courseTypeId; }
int Course::getCreditHours() const { return creditHours; }
int Course::getLectureHours() const { return lectureHours; }
int Course::getTutorialHours() const { return tutorialHours; }
int Course::getLabHours() const { return labHours; }
int Course::getPracticalHours() const { return practicalHours; }
int Course::getMaxMarks() const { return maxMarks; }
int Course::getAssignment1Marks() const { return assignment1Marks; }
int Course::getAssignment2Marks() const { return assignment2Marks; }
int Course::getYearWorkMarks() const { return yearWorkMarks; }
int Course::getFinalExamMarks() const { return finalExamMarks; }

// ========================================
// SETTERS
// ========================================

void Course::setId(const string& id) { this->id = id; }
void Course::setCode(const string& code) { this->code = code; }
void Course::setNameEn(const string& nameEn) { this->nameEn = nameEn; }
void Course::setNameAr(const string& nameAr) { this->nameAr = nameAr; }
void Course::setDescription(const string& description) { this->description = description; }
void Course::setDepartmentId(const string& departmentId) { this->departmentId = departmentId; }
void Course::setAcademicLevelId(const string& academicLevelId) { this->academicLevelId = academicLevelId; }
void Course::setSemesterNumber(int semesterNumber) { this->semesterNumber = semesterNumber; }
void Course::setCourseTypeId(const string& courseTypeId) { this->courseTypeId = courseTypeId; }
void Course::setCreditHours(int creditHours) { this->creditHours = creditHours; }
void Course::setLectureHours(int lectureHours) { this->lectureHours = lectureHours; }
void Course::setTutorialHours(int tutorialHours) { this->tutorialHours = tutorialHours; }
void Course::setLabHours(int labHours) { this->labHours = labHours; }
void Course::setPracticalHours(int practicalHours) { this->practicalHours = practicalHours; }
void Course::setMaxMarks(int maxMarks) { this->maxMarks = maxMarks; }
void Course::setAssignment1Marks(int assignment1Marks) { this->assignment1Marks = assignment1Marks; }
void Course::setAssignment2Marks(int assignment2Marks) { this->assignment2Marks = assignment2Marks; }
void Course::setYearWorkMarks(int yearWorkMarks) { this->yearWorkMarks = yearWorkMarks; }
void Course::setFinalExamMarks(int finalExamMarks) { this->finalExamMarks = finalExamMarks; }

// ========================================
// STATIC DATABASE OPERATIONS
// ========================================

vector<Course> Course::getAllCourses(Database& db) {
    vector<Course> courses;

    try {
        auto result = db.executeQuery(
            "SELECT id, code, name_en, name_ar, description, department_id, "
            "academic_level_id, semester_number, course_type_id, credit_hours, "
            "lecture_hours, tutorial_hours, lab_hours, practical_hours, max_marks, "
            "assignment1_marks, assignment2_marks, year_work_marks, final_exam_marks "
            "FROM courses ORDER BY code"
        );

        while (result->next()) {
            Course course;
            course.id = result->getString("id");
            course.code = result->getString("code");
            course.nameEn = result->getString("name_en");
            course.nameAr = result->getString("name_ar");
            course.description = result->getString("description");
            course.departmentId = result->getString("department_id");
            course.academicLevelId = result->getString("academic_level_id");
            course.semesterNumber = result->getInt("semester_number");
            course.courseTypeId = result->getString("course_type_id");
            course.creditHours = result->getInt("credit_hours");
            course.lectureHours = result->getInt("lecture_hours");
            course.tutorialHours = result->getInt("tutorial_hours");
            course.labHours = result->getInt("lab_hours");
            course.practicalHours = result->getInt("practical_hours");
            course.maxMarks = result->getInt("max_marks");
            course.assignment1Marks = result->getInt("assignment1_marks");
            course.assignment2Marks = result->getInt("assignment2_marks");
            course.yearWorkMarks = result->getInt("year_work_marks");
            course.finalExamMarks = result->getInt("final_exam_marks");

            courses.push_back(course);
        }

    } catch (exception& e) {
        cerr << "[Course] Error getting all courses: " << e.what() << endl;
    }

    return courses;
}

Course Course::getById(Database& db, const string& id) {
    Course course;

    try {
        auto stmt = db.prepareStatement(
            "SELECT id, code, name_en, name_ar, description, department_id, "
            "academic_level_id, semester_number, course_type_id, credit_hours, "
            "lecture_hours, tutorial_hours, lab_hours, practical_hours, max_marks, "
            "assignment1_marks, assignment2_marks, year_work_marks, final_exam_marks "
            "FROM courses WHERE id = ?"
        );
        stmt->setString(1, id);

        auto result = stmt->executeQuery();

        if (result->next()) {
            course.id = result->getString("id");
            course.code = result->getString("code");
            course.nameEn = result->getString("name_en");
            course.nameAr = result->getString("name_ar");
            course.description = result->getString("description");
            course.departmentId = result->getString("department_id");
            course.academicLevelId = result->getString("academic_level_id");
            course.semesterNumber = result->getInt("semester_number");
            course.courseTypeId = result->getString("course_type_id");
            course.creditHours = result->getInt("credit_hours");
            course.lectureHours = result->getInt("lecture_hours");
            course.tutorialHours = result->getInt("tutorial_hours");
            course.labHours = result->getInt("lab_hours");
            course.practicalHours = result->getInt("practical_hours");
            course.maxMarks = result->getInt("max_marks");
            course.assignment1Marks = result->getInt("assignment1_marks");
            course.assignment2Marks = result->getInt("assignment2_marks");
            course.yearWorkMarks = result->getInt("year_work_marks");
            course.finalExamMarks = result->getInt("final_exam_marks");
        }

    } catch (exception& e) {
        cerr << "[Course] Error getting course by ID: " << e.what() << endl;
    }

    return course;
}

Course Course::getByCode(Database& db, const string& code) {
    Course course;

    try {
        auto stmt = db.prepareStatement(
            "SELECT id, code, name_en, name_ar, description, department_id, "
            "academic_level_id, semester_number, course_type_id, credit_hours, "
            "lecture_hours, tutorial_hours, lab_hours, practical_hours, max_marks, "
            "assignment1_marks, assignment2_marks, year_work_marks, final_exam_marks "
            "FROM courses WHERE code = ?"
        );
        stmt->setString(1, code);

        auto result = stmt->executeQuery();

        if (result->next()) {
            course.id = result->getString("id");
            course.code = result->getString("code");
            course.nameEn = result->getString("name_en");
            course.nameAr = result->getString("name_ar");
            course.description = result->getString("description");
            course.departmentId = result->getString("department_id");
            course.academicLevelId = result->getString("academic_level_id");
            course.semesterNumber = result->getInt("semester_number");
            course.courseTypeId = result->getString("course_type_id");
            course.creditHours = result->getInt("credit_hours");
            course.lectureHours = result->getInt("lecture_hours");
            course.tutorialHours = result->getInt("tutorial_hours");
            course.labHours = result->getInt("lab_hours");
            course.practicalHours = result->getInt("practical_hours");
            course.maxMarks = result->getInt("max_marks");
            course.assignment1Marks = result->getInt("assignment1_marks");
            course.assignment2Marks = result->getInt("assignment2_marks");
            course.yearWorkMarks = result->getInt("year_work_marks");
            course.finalExamMarks = result->getInt("final_exam_marks");
        }

    } catch (exception& e) {
        cerr << "[Course] Error getting course by code: " << e.what() << endl;
    }

    return course;
}

bool Course::exists(Database& db, const string& id) {
    try {
        auto stmt = db.prepareStatement("SELECT COUNT(*) as count FROM courses WHERE id = ?");
        stmt->setString(1, id);

        auto result = stmt->executeQuery();

        if (result->next()) {
            return result->getInt("count") > 0;
        }

    } catch (exception& e) {
        cerr << "[Course] Error checking if course exists: " << e.what() << endl;
    }

    return false;
}

bool Course::codeExists(Database& db, const string& code) {
    try {
        auto stmt = db.prepareStatement("SELECT COUNT(*) as count FROM courses WHERE code = ?");
        stmt->setString(1, code);

        auto result = stmt->executeQuery();

        if (result->next()) {
            return result->getInt("count") > 0;
        }

    } catch (exception& e) {
        cerr << "[Course] Error checking if course code exists: " << e.what() << endl;
    }

    return false;
}

vector<Course> Course::getByDepartment(Database& db, const string& departmentId) {
    vector<Course> courses;

    try {
        auto stmt = db.prepareStatement(
            "SELECT id, code, name_en, name_ar, description, department_id, "
            "academic_level_id, semester_number, course_type_id, credit_hours, "
            "lecture_hours, tutorial_hours, lab_hours, practical_hours, max_marks, "
            "assignment1_marks, assignment2_marks, year_work_marks, final_exam_marks "
            "FROM courses WHERE department_id = ? ORDER BY code"
        );
        stmt->setString(1, departmentId);

        auto result = stmt->executeQuery();

        while (result->next()) {
            Course course;
            course.id = result->getString("id");
            course.code = result->getString("code");
            course.nameEn = result->getString("name_en");
            course.nameAr = result->getString("name_ar");
            course.description = result->getString("description");
            course.departmentId = result->getString("department_id");
            course.academicLevelId = result->getString("academic_level_id");
            course.semesterNumber = result->getInt("semester_number");
            course.courseTypeId = result->getString("course_type_id");
            course.creditHours = result->getInt("credit_hours");
            course.lectureHours = result->getInt("lecture_hours");
            course.tutorialHours = result->getInt("tutorial_hours");
            course.labHours = result->getInt("lab_hours");
            course.practicalHours = result->getInt("practical_hours");
            course.maxMarks = result->getInt("max_marks");
            course.assignment1Marks = result->getInt("assignment1_marks");
            course.assignment2Marks = result->getInt("assignment2_marks");
            course.yearWorkMarks = result->getInt("year_work_marks");
            course.finalExamMarks = result->getInt("final_exam_marks");

            courses.push_back(course);
        }

    } catch (exception& e) {
        cerr << "[Course] Error getting courses by department: " << e.what() << endl;
    }

    return courses;
}

vector<Course> Course::getByAcademicLevel(Database& db, const string& levelId) {
    vector<Course> courses;

    try {
        auto stmt = db.prepareStatement(
            "SELECT id, code, name_en, name_ar, description, department_id, "
            "academic_level_id, semester_number, course_type_id, credit_hours, "
            "lecture_hours, tutorial_hours, lab_hours, practical_hours, max_marks, "
            "assignment1_marks, assignment2_marks, year_work_marks, final_exam_marks "
            "FROM courses WHERE academic_level_id = ? ORDER BY code"
        );
        stmt->setString(1, levelId);

        auto result = stmt->executeQuery();

        while (result->next()) {
            Course course;
            course.id = result->getString("id");
            course.code = result->getString("code");
            course.nameEn = result->getString("name_en");
            course.nameAr = result->getString("name_ar");
            course.description = result->getString("description");
            course.departmentId = result->getString("department_id");
            course.academicLevelId = result->getString("academic_level_id");
            course.semesterNumber = result->getInt("semester_number");
            course.courseTypeId = result->getString("course_type_id");
            course.creditHours = result->getInt("credit_hours");
            course.lectureHours = result->getInt("lecture_hours");
            course.tutorialHours = result->getInt("tutorial_hours");
            course.labHours = result->getInt("lab_hours");
            course.practicalHours = result->getInt("practical_hours");
            course.maxMarks = result->getInt("max_marks");
            course.assignment1Marks = result->getInt("assignment1_marks");
            course.assignment2Marks = result->getInt("assignment2_marks");
            course.yearWorkMarks = result->getInt("year_work_marks");
            course.finalExamMarks = result->getInt("final_exam_marks");

            courses.push_back(course);
        }

    } catch (exception& e) {
        cerr << "[Course] Error getting courses by academic level: " << e.what() << endl;
    }

    return courses;
}

// ========================================
// INSTANCE DATABASE OPERATIONS
// ========================================

bool Course::save(Database& db) {
    if (exists(db, id)) {
        cerr << "[Course] Error: Course ID already exists!" << endl;
        return false;
    }

    try {
        auto stmt = db.prepareStatement(
            "INSERT INTO courses (id, code, name_en, name_ar, description, department_id, "
            "academic_level_id, semester_number, course_type_id, credit_hours, lecture_hours, "
            "tutorial_hours, lab_hours, practical_hours, max_marks, assignment1_marks, "
            "assignment2_marks, year_work_marks, final_exam_marks) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
        );
        stmt->setString(1, id);
        stmt->setString(2, code);
        stmt->setString(3, nameEn);
        if (nameAr.empty()) stmt->setNull(4); else stmt->setString(4, nameAr);
        if (description.empty()) stmt->setNull(5); else stmt->setString(5, description);
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
        stmt->setInt(16, assignment1Marks);
        stmt->setInt(17, assignment2Marks);
        stmt->setInt(18, yearWorkMarks);
        stmt->setInt(19, finalExamMarks);

        stmt->executeUpdate();
        return true;

    } catch (exception& e) {
        cerr << "[Course] Error saving course: " << e.what() << endl;
        return false;
    }
}

bool Course::update(Database& db) {
    try {
        auto stmt = db.prepareStatement(
            "UPDATE courses SET code = ?, name_en = ?, name_ar = ?, description = ?, "
            "department_id = ?, academic_level_id = ?, semester_number = ?, course_type_id = ?, "
            "credit_hours = ?, lecture_hours = ?, tutorial_hours = ?, lab_hours = ?, "
            "practical_hours = ?, max_marks = ?, assignment1_marks = ?, assignment2_marks = ?, "
            "year_work_marks = ?, final_exam_marks = ? WHERE id = ?"
        );
        stmt->setString(1, code);
        stmt->setString(2, nameEn);
        if (nameAr.empty()) stmt->setNull(3); else stmt->setString(3, nameAr);
        if (description.empty()) stmt->setNull(4); else stmt->setString(4, description);
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
        stmt->setInt(15, assignment1Marks);
        stmt->setInt(16, assignment2Marks);
        stmt->setInt(17, yearWorkMarks);
        stmt->setInt(18, finalExamMarks);
        stmt->setString(19, id);

        int rowsAffected = stmt->executeUpdate();
        return rowsAffected >= 0;

    } catch (exception& e) {
        cerr << "[Course] Error updating course: " << e.what() << endl;
        return false;
    }
}

bool Course::remove(Database& db) {
    try {
        auto stmt = db.prepareStatement("DELETE FROM courses WHERE id = ?");
        stmt->setString(1, id);
        int rowsAffected = stmt->executeUpdate();
        return rowsAffected > 0;

    } catch (exception& e) {
        cerr << "[Course] Error deleting course: " << e.what() << endl;
        return false;
    }
}

// ========================================
// UTILITY METHODS
// ========================================

string Course::toJson() const {
    stringstream ss;
    ss << "{";
    ss << "\"id\":\"" << id << "\",";
    ss << "\"code\":\"" << code << "\",";
    ss << "\"nameEn\":\"" << nameEn << "\",";
    ss << "\"nameAr\":\"" << nameAr << "\",";
    ss << "\"description\":\"" << description << "\",";
    ss << "\"departmentId\":\"" << departmentId << "\",";
    ss << "\"academicLevelId\":\"" << academicLevelId << "\",";
    ss << "\"semesterNumber\":" << semesterNumber << ",";
    ss << "\"courseTypeId\":\"" << courseTypeId << "\",";
    ss << "\"creditHours\":" << creditHours << ",";
    ss << "\"lectureHours\":" << lectureHours << ",";
    ss << "\"tutorialHours\":" << tutorialHours << ",";
    ss << "\"labHours\":" << labHours << ",";
    ss << "\"practicalHours\":" << practicalHours << ",";
    ss << "\"maxMarks\":" << maxMarks << ",";
    ss << "\"assignment1Marks\":" << assignment1Marks << ",";
    ss << "\"assignment2Marks\":" << assignment2Marks << ",";
    ss << "\"yearWorkMarks\":" << yearWorkMarks << ",";
    ss << "\"finalExamMarks\":" << finalExamMarks;
    ss << "}";
    return ss.str();
}

bool Course::isEmpty() const {
    return id.empty();
}
