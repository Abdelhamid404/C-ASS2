/*
 * ============================================
 * COURSE.H - Course Class Definition
 * ============================================
 * This class represents a course in the system.
 */

#ifndef COURSE_H
#define COURSE_H

#include <string>
#include <vector>
#include "Database.h"

// NOTE: Do NOT use "using namespace std;" in header files as it causes
// conflicts with Windows SDK types like "byte" when used with C++17.

class Course {
private:
    std::string id;              // Unique course ID
    std::string code;            // Course code (e.g., "IT111")
    std::string nameEn;          // Course name (English)
    std::string nameAr;          // Course name (Arabic)
    std::string description;     // Course description
    std::string departmentId;    // Foreign key to departments
    std::string academicLevelId; // Foreign key to academic_levels
    int semesterNumber;          // Semester number (1/2)
    std::string courseTypeId;    // FK to course_types
    int creditHours;             // Credit hours
    int lectureHours;            // Lecture hours
    int tutorialHours;           // Tutorial hours
    int labHours;                // Lab hours
    int practicalHours;          // Practical hours
    int maxMarks;                // Maximum marks
    int assignment1Marks;        // Assignment 1 marks
    int assignment2Marks;        // Assignment 2 marks
    int yearWorkMarks;           // Year work marks
    int finalExamMarks;          // Final exam marks

public:
    // Constructors
    Course();
    Course(const std::string& id, const std::string& code, const std::string& nameEn,
           const std::string& nameAr, const std::string& description,
           const std::string& departmentId, const std::string& academicLevelId,
           int semesterNumber, const std::string& courseTypeId, int creditHours,
           int lectureHours, int tutorialHours, int labHours, int practicalHours,
           int maxMarks, int assignment1Marks, int assignment2Marks, int yearWorkMarks,
           int finalExamMarks);

    // Getters
    std::string getId() const;
    std::string getCode() const;
    std::string getNameEn() const;
    std::string getNameAr() const;
    std::string getDescription() const;
    std::string getDepartmentId() const;
    std::string getAcademicLevelId() const;
    int getSemesterNumber() const;
    std::string getCourseTypeId() const;
    int getCreditHours() const;
    int getLectureHours() const;
    int getTutorialHours() const;
    int getLabHours() const;
    int getPracticalHours() const;
    int getMaxMarks() const;
    int getAssignment1Marks() const;
    int getAssignment2Marks() const;
    int getYearWorkMarks() const;
    int getFinalExamMarks() const;

    // Setters
    void setId(const std::string& id);
    void setCode(const std::string& code);
    void setNameEn(const std::string& nameEn);
    void setNameAr(const std::string& nameAr);
    void setDescription(const std::string& description);
    void setDepartmentId(const std::string& departmentId);
    void setAcademicLevelId(const std::string& academicLevelId);
    void setSemesterNumber(int semesterNumber);
    void setCourseTypeId(const std::string& courseTypeId);
    void setCreditHours(int creditHours);
    void setLectureHours(int lectureHours);
    void setTutorialHours(int tutorialHours);
    void setLabHours(int labHours);
    void setPracticalHours(int practicalHours);
    void setMaxMarks(int maxMarks);
    void setAssignment1Marks(int assignment1Marks);
    void setAssignment2Marks(int assignment2Marks);
    void setYearWorkMarks(int yearWorkMarks);
    void setFinalExamMarks(int finalExamMarks);

    // Static database operations
    static std::vector<Course> getAllCourses(Database& db);
    static Course getById(Database& db, const std::string& id);
    static Course getByCode(Database& db, const std::string& code);
    static bool exists(Database& db, const std::string& id);
    static bool codeExists(Database& db, const std::string& code);
    static std::vector<Course> getByDepartment(Database& db, const std::string& departmentId);
    static std::vector<Course> getByAcademicLevel(Database& db, const std::string& levelId);

    // Instance database operations
    bool save(Database& db);
    bool update(Database& db);
    bool remove(Database& db);

    // Utility methods
    std::string toJson() const;
    bool isEmpty() const;
};

#endif // COURSE_H
