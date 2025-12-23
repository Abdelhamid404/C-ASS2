/*
 * ============================================
 * GRADE.CPP - Grade Class Implementation
 * ============================================
 */

#include "../include/Grade.h"
#include <sstream>
#include <iomanip>

using namespace std;

// ========================================
// CONSTRUCTORS
// ========================================

Grade::Grade()
    : id(""), registrationId(""), assignment1(0), assignment2(0), yearWork(0),
      finalExam(0), total(0), percentage(0), gpa(0), evaluation("Fail"),
      letterGrade("F") {
}

Grade::Grade(const string& id, const string& registrationId)
    : id(id), registrationId(registrationId), assignment1(0), assignment2(0),
      yearWork(0), finalExam(0), total(0), percentage(0), gpa(0),
      evaluation("Fail"), letterGrade("F") {
}

// ========================================
// GETTERS
// ========================================

string Grade::getId() const { return id; }
string Grade::getRegistrationId() const { return registrationId; }
double Grade::getAssignment1() const { return assignment1; }
double Grade::getAssignment2() const { return assignment2; }
double Grade::getYearWork() const { return yearWork; }
double Grade::getFinalExam() const { return finalExam; }
double Grade::getTotal() const { return total; }
double Grade::getPercentage() const { return percentage; }
double Grade::getGpa() const { return gpa; }
string Grade::getEvaluation() const { return evaluation; }
string Grade::getLetterGrade() const { return letterGrade; }

// ========================================
// SETTERS
// ========================================

void Grade::setId(const string& id) { this->id = id; }
void Grade::setRegistrationId(const string& registrationId) { this->registrationId = registrationId; }
void Grade::setAssignment1(double assignment1) { this->assignment1 = assignment1; }
void Grade::setAssignment2(double assignment2) { this->assignment2 = assignment2; }
void Grade::setYearWork(double yearWork) { this->yearWork = yearWork; }
void Grade::setFinalExam(double finalExam) { this->finalExam = finalExam; }

// ========================================
// STATIC HELPER FUNCTIONS
// ========================================

string Grade::getEvaluationFromPercentage(double percentage) {
    if (percentage >= 85) return "Excellent";
    if (percentage >= 75) return "Very Good";
    if (percentage >= 65) return "Good";
    if (percentage >= 60) return "Pass";
    return "Fail";
}

double Grade::getGpaFromPercentage(double percentage) {
    if (percentage >= 85) return 4.0;
    if (percentage >= 75) return 3.0;
    if (percentage >= 65) return 2.0;
    if (percentage >= 60) return 1.0;
    return 0.0;
}

string Grade::getLetterGrade(double percentage) {
    if (percentage >= 85) return "A";
    if (percentage >= 75) return "B";
    if (percentage >= 65) return "C";
    if (percentage >= 60) return "D";
    return "F";
}

Grade::GradeDistribution Grade::getDistributionForRegistration(Database& db,
                                                                const string& registrationId) {
    GradeDistribution dist{0.0, 0.0, 0.0, 0.0, 0.0};

    try {
        auto stmt = db.prepareStatement(
            "SELECT c.max_marks, c.assignment1_marks, c.assignment2_marks, c.year_work_marks, c.final_exam_marks "
            "FROM courses c "
            "JOIN registrations r ON r.course_id = c.id "
            "WHERE r.id = ?"
        );
        stmt->setString(1, registrationId);
        auto result = stmt->executeQuery();
        if (result->next()) {
            dist.assignment1Max = result->getDouble("assignment1_marks");
            dist.assignment2Max = result->getDouble("assignment2_marks");
            dist.yearWorkMax = result->getDouble("year_work_marks");
            dist.finalExamMax = result->getDouble("final_exam_marks");
            dist.totalMax = result->getDouble("max_marks");
            if (dist.totalMax <= 0) {
                dist.totalMax = dist.assignment1Max + dist.assignment2Max + dist.yearWorkMax + dist.finalExamMax;
            }
        }
    } catch (const exception& e) {
        cerr << "[Grade] Error loading distribution: " << e.what() << endl;
    }

    return dist;
}

// ========================================
// VALIDATION
// ========================================

string Grade::validateGradeComponent(Database& db, const string& registrationId,
                                     const string& component, double value) {
    try {
        GradeDistribution dist = getDistributionForRegistration(db, registrationId);

        double maxValue = 0;
        string componentName;

        if (component == "assignment1") {
            maxValue = dist.assignment1Max;
            componentName = "Assignment 1";
        } else if (component == "assignment2") {
            maxValue = dist.assignment2Max;
            componentName = "Assignment 2";
        } else if (component == "year_work") {
            maxValue = dist.yearWorkMax;
            componentName = "Year Work";
        } else if (component == "final_exam") {
            maxValue = dist.finalExamMax;
            componentName = "Final Exam";
        } else {
            return "Invalid grade component";
        }

        if (value < 0) {
            return componentName + " cannot be negative";
        }
        if (value > maxValue) {
            stringstream ss;
            ss << componentName << " cannot exceed " << fixed << setprecision(1) << maxValue;
            return ss.str();
        }

        return "";

    } catch (const exception& e) {
        return string("Validation error: ") + e.what();
    }
}

// ========================================
// STATIC DATABASE OPERATIONS
// ========================================

vector<Grade> Grade::getAllGrades(Database& db) {
    vector<Grade> grades;

    try {
        auto result = db.executeQuery(
            "SELECT id, registration_id, assignment1, assignment2, year_work, final_exam, "
            "total_marks, percentage, gpa, evaluation, letter_grade "
            "FROM grades ORDER BY id"
        );

        while (result->next()) {
            Grade grade;
            grade.id = result->getString("id");
            grade.registrationId = result->getString("registration_id");
            grade.assignment1 = result->getDouble("assignment1");
            grade.assignment2 = result->getDouble("assignment2");
            grade.yearWork = result->getDouble("year_work");
            grade.finalExam = result->getDouble("final_exam");
            grade.total = result->getDouble("total_marks");
            grade.percentage = result->getDouble("percentage");
            grade.gpa = result->getDouble("gpa");
            grade.evaluation = result->getString("evaluation");
            grade.letterGrade = result->getString("letter_grade");

            grades.push_back(grade);
        }

    } catch (exception& e) {
        cerr << "[Grade] Error getting all grades: " << e.what() << endl;
    }

    return grades;
}

Grade Grade::getById(Database& db, const string& id) {
    Grade grade;

    try {
        auto stmt = db.prepareStatement(
            "SELECT id, registration_id, assignment1, assignment2, year_work, final_exam, "
            "total_marks, percentage, gpa, evaluation, letter_grade "
            "FROM grades WHERE id = ?"
        );
        stmt->setString(1, id);

        auto result = stmt->executeQuery();

        if (result->next()) {
            grade.id = result->getString("id");
            grade.registrationId = result->getString("registration_id");
            grade.assignment1 = result->getDouble("assignment1");
            grade.assignment2 = result->getDouble("assignment2");
            grade.yearWork = result->getDouble("year_work");
            grade.finalExam = result->getDouble("final_exam");
            grade.total = result->getDouble("total_marks");
            grade.percentage = result->getDouble("percentage");
            grade.gpa = result->getDouble("gpa");
            grade.evaluation = result->getString("evaluation");
            grade.letterGrade = result->getString("letter_grade");
        }

    } catch (exception& e) {
        cerr << "[Grade] Error getting grade by ID: " << e.what() << endl;
    }

    return grade;
}

Grade Grade::getByRegistrationId(Database& db, const string& registrationId) {
    Grade grade;

    try {
        auto stmt = db.prepareStatement(
            "SELECT id, registration_id, assignment1, assignment2, year_work, final_exam, "
            "total_marks, percentage, gpa, evaluation, letter_grade "
            "FROM grades WHERE registration_id = ?"
        );
        stmt->setString(1, registrationId);

        auto result = stmt->executeQuery();

        if (result->next()) {
            grade.id = result->getString("id");
            grade.registrationId = result->getString("registration_id");
            grade.assignment1 = result->getDouble("assignment1");
            grade.assignment2 = result->getDouble("assignment2");
            grade.yearWork = result->getDouble("year_work");
            grade.finalExam = result->getDouble("final_exam");
            grade.total = result->getDouble("total_marks");
            grade.percentage = result->getDouble("percentage");
            grade.gpa = result->getDouble("gpa");
            grade.evaluation = result->getString("evaluation");
            grade.letterGrade = result->getString("letter_grade");
        }

    } catch (exception& e) {
        cerr << "[Grade] Error getting grade by registration: " << e.what() << endl;
    }

    return grade;
}

vector<Grade> Grade::getByStudentId(Database& db, const string& studentId) {
    vector<Grade> grades;

    try {
        auto stmt = db.prepareStatement(
            "SELECT g.id, g.registration_id, g.assignment1, g.assignment2, g.year_work, g.final_exam, "
            "g.total_marks, g.percentage, g.gpa, g.evaluation, g.letter_grade "
            "FROM grades g "
            "JOIN registrations r ON g.registration_id = r.id "
            "WHERE r.student_id = ?"
        );
        stmt->setString(1, studentId);

        auto result = stmt->executeQuery();
        while (result->next()) {
            Grade grade;
            grade.id = result->getString("id");
            grade.registrationId = result->getString("registration_id");
            grade.assignment1 = result->getDouble("assignment1");
            grade.assignment2 = result->getDouble("assignment2");
            grade.yearWork = result->getDouble("year_work");
            grade.finalExam = result->getDouble("final_exam");
            grade.total = result->getDouble("total_marks");
            grade.percentage = result->getDouble("percentage");
            grade.gpa = result->getDouble("gpa");
            grade.evaluation = result->getString("evaluation");
            grade.letterGrade = result->getString("letter_grade");

            grades.push_back(grade);
        }

    } catch (exception& e) {
        cerr << "[Grade] Error getting grades by student: " << e.what() << endl;
    }

    return grades;
}

vector<Grade> Grade::getByCourseId(Database& db, const string& courseId,
                                   const string& semesterId) {
    vector<Grade> grades;

    try {
        auto stmt = db.prepareStatement(
            "SELECT g.id, g.registration_id, g.assignment1, g.assignment2, g.year_work, g.final_exam, "
            "g.total_marks, g.percentage, g.gpa, g.evaluation, g.letter_grade "
            "FROM grades g "
            "JOIN registrations r ON g.registration_id = r.id "
            "WHERE r.course_id = ? AND r.semester_id = ?"
        );
        stmt->setString(1, courseId);
        stmt->setString(2, semesterId);

        auto result = stmt->executeQuery();
        while (result->next()) {
            Grade grade;
            grade.id = result->getString("id");
            grade.registrationId = result->getString("registration_id");
            grade.assignment1 = result->getDouble("assignment1");
            grade.assignment2 = result->getDouble("assignment2");
            grade.yearWork = result->getDouble("year_work");
            grade.finalExam = result->getDouble("final_exam");
            grade.total = result->getDouble("total_marks");
            grade.percentage = result->getDouble("percentage");
            grade.gpa = result->getDouble("gpa");
            grade.evaluation = result->getString("evaluation");
            grade.letterGrade = result->getString("letter_grade");

            grades.push_back(grade);
        }

    } catch (exception& e) {
        cerr << "[Grade] Error getting grades by course: " << e.what() << endl;
    }

    return grades;
}

bool Grade::calculateGrade(Database& db, const string& registrationId) {
    try {
        auto stmt = db.prepareStatement(
            "SELECT g.id, g.assignment1, g.assignment2, g.year_work, g.final_exam, "
            "c.max_marks "
            "FROM grades g "
            "JOIN registrations r ON g.registration_id = r.id "
            "JOIN courses c ON r.course_id = c.id "
            "WHERE g.registration_id = ?"
        );
        stmt->setString(1, registrationId);
        auto result = stmt->executeQuery();

        if (!result->next()) {
            return false;
        }

        double assignment1 = result->getDouble("assignment1");
        double assignment2 = result->getDouble("assignment2");
        double yearWork = result->getDouble("year_work");
        double finalExam = result->getDouble("final_exam");
        double maxMarks = result->getDouble("max_marks");

        double total = assignment1 + assignment2 + yearWork + finalExam;
        double percentage = (maxMarks > 0) ? (total / maxMarks * 100.0) : 0.0;
        string evaluation = getEvaluationFromPercentage(percentage);
        double gpa = getGpaFromPercentage(percentage);
        string letter = getLetterGrade(percentage);

        auto updateStmt = db.prepareStatement(
            "UPDATE grades SET total_marks = ?, percentage = ?, gpa = ?, evaluation = ?, letter_grade = ? "
            "WHERE registration_id = ?"
        );
        updateStmt->setDouble(1, total);
        updateStmt->setDouble(2, percentage);
        updateStmt->setDouble(3, gpa);
        updateStmt->setString(4, evaluation);
        updateStmt->setString(5, letter);
        updateStmt->setString(6, registrationId);
        updateStmt->executeUpdate();

        return true;

    } catch (exception& e) {
        cerr << "[Grade] Error calculating grade: " << e.what() << endl;
        return false;
    }
}

double Grade::calculateStudentCGPA(Database& db, const string& studentId) {
    try {
        auto stmt = db.prepareStatement(
            "SELECT COALESCE(SUM(g.gpa * c.credit_hours), 0) as total_points, "
            "COALESCE(SUM(c.credit_hours), 0) as total_credits "
            "FROM grades g "
            "JOIN registrations r ON g.registration_id = r.id "
            "JOIN courses c ON r.course_id = c.id "
            "WHERE r.student_id = ? AND r.status IN ('registered', 'completed')"
        );
        stmt->setString(1, studentId);

        auto result = stmt->executeQuery();
        if (result->next()) {
            double totalPoints = result->getDouble("total_points");
            int totalCredits = result->getInt("total_credits");

            if (totalCredits > 0) {
                return totalPoints / totalCredits;
            }
        }

        return 0.0;

    } catch (exception& e) {
        cerr << "[Grade] Error calculating CGPA: " << e.what() << endl;
        return 0.0;
    }
}

double Grade::calculateStudentSemesterGPA(Database& db, const string& studentId,
                                          const string& semesterId) {
    try {
        auto stmt = db.prepareStatement(
            "SELECT COALESCE(SUM(g.gpa * c.credit_hours), 0) as total_points, "
            "COALESCE(SUM(c.credit_hours), 0) as total_credits "
            "FROM grades g "
            "JOIN registrations r ON g.registration_id = r.id "
            "JOIN courses c ON r.course_id = c.id "
            "WHERE r.student_id = ? AND r.semester_id = ?"
        );
        stmt->setString(1, studentId);
        stmt->setString(2, semesterId);

        auto result = stmt->executeQuery();
        if (result->next()) {
            double totalPoints = result->getDouble("total_points");
            int totalCredits = result->getInt("total_credits");

            if (totalCredits > 0) {
                return totalPoints / totalCredits;
            }
        }

        return 0.0;

    } catch (exception& e) {
        cerr << "[Grade] Error calculating semester GPA: " << e.what() << endl;
        return 0.0;
    }
}

// ========================================
// INSTANCE DATABASE OPERATIONS
// ========================================

bool Grade::save(Database& db) {
    try {
        auto stmt = db.prepareStatement(
            "INSERT INTO grades (registration_id, assignment1, assignment2, year_work, final_exam) "
            "VALUES (?, ?, ?, ?, ?)"
        );
        stmt->setString(1, registrationId);
        stmt->setDouble(2, assignment1);
        stmt->setDouble(3, assignment2);
        stmt->setDouble(4, yearWork);
        stmt->setDouble(5, finalExam);
        stmt->executeUpdate();

        id = to_string(db.getLastInsertId());
        calculateGrade(db, registrationId);
        return true;

    } catch (exception& e) {
        cerr << "[Grade] Error saving grade: " << e.what() << endl;
        return false;
    }
}

bool Grade::update(Database& db) {
    try {
        auto stmt = db.prepareStatement(
            "UPDATE grades SET assignment1 = ?, assignment2 = ?, year_work = ?, final_exam = ? "
            "WHERE registration_id = ?"
        );
        stmt->setDouble(1, assignment1);
        stmt->setDouble(2, assignment2);
        stmt->setDouble(3, yearWork);
        stmt->setDouble(4, finalExam);
        stmt->setString(5, registrationId);

        int rowsAffected = stmt->executeUpdate();
        calculateGrade(db, registrationId);
        return rowsAffected > 0;

    } catch (exception& e) {
        cerr << "[Grade] Error updating grade: " << e.what() << endl;
        return false;
    }
}

bool Grade::updateComponent(Database& db, const string& component, double value) {
    string validationError = validateGradeComponent(db, registrationId, component, value);
    if (!validationError.empty()) {
        cerr << "[Grade] Validation failed: " << validationError << endl;
        return false;
    }

    string column;
    if (component == "assignment1") column = "assignment1";
    else if (component == "assignment2") column = "assignment2";
    else if (component == "year_work") column = "year_work";
    else if (component == "final_exam") column = "final_exam";
    else return false;

    try {
        string query = "UPDATE grades SET " + column + " = ? WHERE registration_id = ?";
        auto stmt = db.prepareStatement(query);
        stmt->setDouble(1, value);
        stmt->setString(2, registrationId);
        int rowsAffected = stmt->executeUpdate();

        calculateGrade(db, registrationId);
        return rowsAffected > 0;

    } catch (exception& e) {
        cerr << "[Grade] Error updating grade component: " << e.what() << endl;
        return false;
    }
}

bool Grade::remove(Database& db) {
    try {
        auto stmt = db.prepareStatement("DELETE FROM grades WHERE id = ?");
        stmt->setString(1, id);

        int rowsAffected = stmt->executeUpdate();
        return rowsAffected > 0;

    } catch (exception& e) {
        cerr << "[Grade] Error deleting grade: " << e.what() << endl;
        return false;
    }
}

// ========================================
// UTILITY
// ========================================

string Grade::toJson() const {
    stringstream ss;
    ss << "{";
    ss << "\"id\":\"" << id << "\",";
    ss << "\"registrationId\":\"" << registrationId << "\",";
    ss << "\"assignment1\":" << assignment1 << ",";
    ss << "\"assignment2\":" << assignment2 << ",";
    ss << "\"yearWork\":" << yearWork << ",";
    ss << "\"finalExam\":" << finalExam << ",";
    ss << "\"total\":" << total << ",";
    ss << "\"percentage\":" << percentage << ",";
    ss << "\"gpa\":" << gpa << ",";
    ss << "\"evaluation\":\"" << evaluation << "\",";
    ss << "\"letterGrade\":\"" << letterGrade << "\"";
    ss << "}";
    return ss.str();
}

bool Grade::isEmpty() const {
    return id.empty();
}
