/*
 * ============================================
 * SECTION.H - Section Class Definition
 * ============================================
 */

#ifndef SECTION_H
#define SECTION_H

#include <string>
#include <vector>
#include "Database.h"

class Section {
private:
    std::string id;
    std::string courseId;
    std::string sectionType;  // lecture, lab, tutorial
    std::string hallId;
    std::string labId;
    std::string professorId;
    std::string scheduleId;
    int maxStudents;

public:
    Section();
    Section(const std::string& id, const std::string& courseId,
            const std::string& sectionType, const std::string& professorId);

    // Getters
    std::string getId() const;
    std::string getCourseId() const;
    std::string getSectionType() const;
    std::string getHallId() const;
    std::string getLabId() const;
    std::string getProfessorId() const;
    std::string getScheduleId() const;
    int getMaxStudents() const;

    // Setters
    void setId(const std::string& id);
    void setCourseId(const std::string& courseId);
    void setSectionType(const std::string& sectionType);
    void setHallId(const std::string& hallId);
    void setLabId(const std::string& labId);
    void setProfessorId(const std::string& professorId);
    void setScheduleId(const std::string& scheduleId);
    void setMaxStudents(int maxStudents);

    // Static database operations
    static std::vector<Section> getAllSections(Database& db);
    static Section getById(Database& db, const std::string& id);
    static std::vector<Section> getByCourseId(Database& db, const std::string& courseId);
    static std::vector<Section> getByProfessorId(Database& db, const std::string& professorId);
    static bool exists(Database& db, const std::string& id);

    // Instance database operations
    bool save(Database& db);
    bool update(Database& db);
    bool remove(Database& db);

    // Utility methods
    std::string toJson() const;
    bool isEmpty() const;
};

#endif // SECTION_H
