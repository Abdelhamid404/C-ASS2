/*
 * ============================================
 * GRADE.H - Grade Class Definition v3
 * ============================================
 * Handles grading with course-based distributions.
 */

#ifndef GRADE_H
#define GRADE_H

#include <string>
#include <vector>
#include "Database.h"

class Grade {
private:
    std::string id;
    std::string registrationId;
    double assignment1;
    double assignment2;
    double yearWork;
    double finalExam;
    double total;
    double percentage;
    double gpa;
    std::string evaluation;
    std::string letterGrade;

public:
    struct GradeDistribution {
        double assignment1Max;
        double assignment2Max;
        double yearWorkMax;
        double finalExamMax;
        double totalMax;
    };

    // Constructors
    Grade();
    Grade(const std::string& id, const std::string& registrationId);

    // Getters
    std::string getId() const;
    std::string getRegistrationId() const;
    double getAssignment1() const;
    double getAssignment2() const;
    double getYearWork() const;
    double getFinalExam() const;
    double getTotal() const;
    double getPercentage() const;
    double getGpa() const;
    std::string getEvaluation() const;
    std::string getLetterGrade() const;

    // Setters
    void setId(const std::string& id);
    void setRegistrationId(const std::string& registrationId);
    void setAssignment1(double assignment1);
    void setAssignment2(double assignment2);
    void setYearWork(double yearWork);
    void setFinalExam(double finalExam);

    // Static helper methods
    static std::string getEvaluationFromPercentage(double percentage);
    static double getGpaFromPercentage(double percentage);
    static std::string getLetterGrade(double percentage);

    static GradeDistribution getDistributionForRegistration(Database& db,
                                                            const std::string& registrationId);

    // Validation (returns empty string if valid, error message if invalid)
    static std::string validateGradeComponent(Database& db, const std::string& registrationId,
                                              const std::string& component, double value);

    // Static database operations
    static std::vector<Grade> getAllGrades(Database& db);
    static Grade getById(Database& db, const std::string& id);
    static Grade getByRegistrationId(Database& db, const std::string& registrationId);
    static std::vector<Grade> getByStudentId(Database& db, const std::string& studentId);
    static std::vector<Grade> getByCourseId(Database& db, const std::string& courseId,
                                            const std::string& semesterId);
    static bool calculateGrade(Database& db, const std::string& registrationId);
    static double calculateStudentCGPA(Database& db, const std::string& studentId);
    static double calculateStudentSemesterGPA(Database& db, const std::string& studentId,
                                              const std::string& semesterId);

    // Instance database operations
    bool save(Database& db);
    bool update(Database& db);
    bool updateComponent(Database& db, const std::string& component, double value);
    bool remove(Database& db);

    // Utility
    std::string toJson() const;
    bool isEmpty() const;
};

#endif // GRADE_H
