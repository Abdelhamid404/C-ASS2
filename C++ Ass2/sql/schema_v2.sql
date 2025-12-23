-- ============================================
-- Student Information System (SIS) Database v2.0
-- NCTU - New Cairo Technological University
-- With RBAC, Proper Validation & Real Data
-- ============================================

DROP DATABASE IF EXISTS nctu_sis;

CREATE DATABASE nctu_sis
CHARACTER SET utf8mb4
COLLATE utf8mb4_unicode_ci;

USE nctu_sis;

-- ============================================
-- TABLE: roles (Role-Based Access Control)
-- ============================================
CREATE TABLE roles (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(50) NOT NULL UNIQUE,
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: permissions
-- ============================================
CREATE TABLE permissions (
    id VARCHAR(30) PRIMARY KEY,
    name VARCHAR(50) NOT NULL UNIQUE,
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: role_permissions (Many-to-Many)
-- ============================================
CREATE TABLE role_permissions (
    role_id VARCHAR(20) NOT NULL,
    permission_id VARCHAR(30) NOT NULL,
    PRIMARY KEY (role_id, permission_id),
    FOREIGN KEY (role_id) REFERENCES roles(id) ON DELETE CASCADE,
    FOREIGN KEY (permission_id) REFERENCES permissions(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: users (Unified user table)
-- ============================================
CREATE TABLE users (
    id VARCHAR(20) PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    full_name VARCHAR(100) NOT NULL,
    email VARCHAR(100),
    phone VARCHAR(20),
    role_id VARCHAR(20) NOT NULL,
    is_active BOOLEAN DEFAULT TRUE,
    last_login TIMESTAMP NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (role_id) REFERENCES roles(id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: academic_levels
-- ============================================
CREATE TABLE academic_levels (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
    year INT NOT NULL CHECK (year BETWEEN 1 AND 6),
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: colleges
-- ============================================
CREATE TABLE colleges (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    name_ar VARCHAR(100),
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: departments
-- ============================================
CREATE TABLE departments (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    name_ar VARCHAR(100),
    college_id VARCHAR(20) NOT NULL,
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (college_id) REFERENCES colleges(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: students (Extended from users)
-- ============================================
CREATE TABLE students (
    id VARCHAR(20) PRIMARY KEY,
    user_id VARCHAR(20) UNIQUE,
    first_name VARCHAR(50) NOT NULL,
    last_name VARCHAR(50) NOT NULL,
    date_of_birth DATE,
    gender ENUM('Male', 'Female') NOT NULL,
    national_id VARCHAR(20),
    academic_level_id VARCHAR(20),
    department_id VARCHAR(20),
    enrollment_date DATE DEFAULT (CURRENT_DATE),
    status ENUM('Active', 'Suspended', 'Graduated', 'Withdrawn') DEFAULT 'Active',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE SET NULL,
    FOREIGN KEY (academic_level_id) REFERENCES academic_levels(id) ON DELETE SET NULL,
    FOREIGN KEY (department_id) REFERENCES departments(id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: professors (Extended from users)
-- ============================================
CREATE TABLE professors (
    id VARCHAR(20) PRIMARY KEY,
    user_id VARCHAR(20) UNIQUE,
    first_name VARCHAR(50) NOT NULL,
    last_name VARCHAR(50) NOT NULL,
    department_id VARCHAR(20),
    specialization VARCHAR(100),
    hire_date DATE DEFAULT (CURRENT_DATE),
    academic_title ENUM('Professor', 'Associate Professor', 'Assistant Professor', 'Lecturer', 'Teaching Assistant') DEFAULT 'Lecturer',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE SET NULL,
    FOREIGN KEY (department_id) REFERENCES departments(id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: courses
-- ============================================
CREATE TABLE courses (
    id VARCHAR(20) PRIMARY KEY,
    code VARCHAR(20) NOT NULL UNIQUE,
    name VARCHAR(100) NOT NULL,
    name_ar VARCHAR(100),
    description TEXT,
    department_id VARCHAR(20),
    academic_level_id VARCHAR(20),
    course_type ENUM('theoretical', 'practical', 'mixed') NOT NULL DEFAULT 'theoretical',
    max_marks INT NOT NULL DEFAULT 100 CHECK (max_marks > 0),
    credit_hours INT NOT NULL DEFAULT 3 CHECK (credit_hours BETWEEN 1 AND 6),
    -- Grade distribution (must sum to max_marks)
    assignment1_max DECIMAL(5,2) DEFAULT 10,
    assignment2_max DECIMAL(5,2) DEFAULT 10,
    coursework_max DECIMAL(5,2) DEFAULT 20,
    final_exam_max DECIMAL(5,2) DEFAULT 60,
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (department_id) REFERENCES departments(id) ON DELETE SET NULL,
    FOREIGN KEY (academic_level_id) REFERENCES academic_levels(id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: course_assignments (Professor -> Course)
-- ============================================
CREATE TABLE course_assignments (
    id VARCHAR(20) PRIMARY KEY,
    professor_id VARCHAR(20) NOT NULL,
    course_id VARCHAR(20) NOT NULL,
    academic_year VARCHAR(20) NOT NULL,
    semester ENUM('Fall', 'Spring', 'Summer') NOT NULL,
    is_primary BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (professor_id) REFERENCES professors(id) ON DELETE CASCADE,
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE,
    UNIQUE KEY unique_assignment (professor_id, course_id, academic_year, semester)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: semesters
-- ============================================
CREATE TABLE semesters (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    academic_year VARCHAR(20) NOT NULL,
    semester_type ENUM('Fall', 'Spring', 'Summer') NOT NULL,
    start_date DATE,
    end_date DATE,
    is_active BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: lecture_halls
-- ============================================
CREATE TABLE lecture_halls (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
    building VARCHAR(50),
    floor INT,
    max_capacity INT NOT NULL CHECK (max_capacity > 0),
    has_projector BOOLEAN DEFAULT TRUE,
    has_ac BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: laboratories
-- ============================================
CREATE TABLE laboratories (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
    building VARCHAR(50),
    floor INT,
    max_capacity INT NOT NULL CHECK (max_capacity > 0),
    lab_type ENUM('Computer', 'Physics', 'Chemistry', 'Electronics', 'Mechatronics', 'Energy', 'Other') DEFAULT 'Computer',
    equipment_count INT DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: registrations
-- ============================================
CREATE TABLE registrations (
    id VARCHAR(20) PRIMARY KEY,
    student_id VARCHAR(20) NOT NULL,
    course_id VARCHAR(20) NOT NULL,
    academic_year VARCHAR(20) NOT NULL,
    semester ENUM('Fall', 'Spring', 'Summer') NOT NULL,
    registration_date DATE DEFAULT (CURRENT_DATE),
    status ENUM('Active', 'Dropped', 'Completed', 'Failed') DEFAULT 'Active',
    registered_by VARCHAR(20),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (student_id) REFERENCES students(id) ON DELETE CASCADE,
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE,
    FOREIGN KEY (registered_by) REFERENCES users(id) ON DELETE SET NULL,
    UNIQUE KEY unique_registration (student_id, course_id, academic_year, semester)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: grades (With validation)
-- ============================================
CREATE TABLE grades (
    id VARCHAR(20) PRIMARY KEY,
    registration_id VARCHAR(20) NOT NULL UNIQUE,
    assignment1 DECIMAL(5,2) DEFAULT 0,
    assignment2 DECIMAL(5,2) DEFAULT 0,
    coursework DECIMAL(5,2) DEFAULT 0,
    final_exam DECIMAL(5,2) DEFAULT 0,
    total DECIMAL(5,2) DEFAULT 0,
    percentage DECIMAL(5,2) DEFAULT 0,
    gpa DECIMAL(3,2) DEFAULT 0,
    letter_grade VARCHAR(2) DEFAULT 'F',
    evaluation ENUM('Excellent', 'Very Good', 'Good', 'Pass', 'Fail') DEFAULT 'Fail',
    entered_by VARCHAR(20),
    last_modified_by VARCHAR(20),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (registration_id) REFERENCES registrations(id) ON DELETE CASCADE,
    FOREIGN KEY (entered_by) REFERENCES users(id) ON DELETE SET NULL,
    FOREIGN KEY (last_modified_by) REFERENCES users(id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: attendance (Date-specific)
-- ============================================
CREATE TABLE attendance (
    id VARCHAR(20) PRIMARY KEY,
    registration_id VARCHAR(20) NOT NULL,
    attendance_date DATE NOT NULL,
    status ENUM('Present', 'Absent', 'Late', 'Excused') NOT NULL,
    remarks TEXT,
    recorded_by VARCHAR(20),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (registration_id) REFERENCES registrations(id) ON DELETE CASCADE,
    FOREIGN KEY (recorded_by) REFERENCES users(id) ON DELETE SET NULL,
    UNIQUE KEY unique_attendance (registration_id, attendance_date)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: audit_log (For tracking changes)
-- ============================================
CREATE TABLE audit_log (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    user_id VARCHAR(20),
    action VARCHAR(50) NOT NULL,
    table_name VARCHAR(50),
    record_id VARCHAR(50),
    old_values JSON,
    new_values JSON,
    ip_address VARCHAR(50),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- STORED PROCEDURE: Calculate Grade with Validation
-- ============================================
DELIMITER //

CREATE PROCEDURE sp_calculate_grade_v2(IN p_registration_id VARCHAR(20))
BEGIN
    DECLARE v_ass1 DECIMAL(5,2);
    DECLARE v_ass2 DECIMAL(5,2);
    DECLARE v_cw DECIMAL(5,2);
    DECLARE v_final DECIMAL(5,2);
    DECLARE v_max_marks INT;
    DECLARE v_total DECIMAL(5,2);
    DECLARE v_percentage DECIMAL(5,2);
    DECLARE v_gpa DECIMAL(3,2);
    DECLARE v_letter VARCHAR(2);
    DECLARE v_evaluation VARCHAR(20);
    
    -- Get grade values and course max marks
    SELECT g.assignment1, g.assignment2, g.coursework, g.final_exam, c.max_marks
    INTO v_ass1, v_ass2, v_cw, v_final, v_max_marks
    FROM grades g
    JOIN registrations r ON g.registration_id = r.id
    JOIN courses c ON r.course_id = c.id
    WHERE g.registration_id = p_registration_id;
    
    -- Calculate total (capped at max_marks)
    SET v_total = LEAST(v_ass1 + v_ass2 + v_cw + v_final, v_max_marks);
    
    -- Calculate percentage (capped at 100%)
    SET v_percentage = LEAST((v_total / v_max_marks) * 100, 100);
    
    -- Calculate GPA and Letter Grade
    IF v_percentage >= 90 THEN
        SET v_gpa = 4.0;
        SET v_letter = 'A+';
        SET v_evaluation = 'Excellent';
    ELSEIF v_percentage >= 85 THEN
        SET v_gpa = 3.7;
        SET v_letter = 'A';
        SET v_evaluation = 'Excellent';
    ELSEIF v_percentage >= 80 THEN
        SET v_gpa = 3.3;
        SET v_letter = 'B+';
        SET v_evaluation = 'Very Good';
    ELSEIF v_percentage >= 75 THEN
        SET v_gpa = 3.0;
        SET v_letter = 'B';
        SET v_evaluation = 'Very Good';
    ELSEIF v_percentage >= 70 THEN
        SET v_gpa = 2.7;
        SET v_letter = 'C+';
        SET v_evaluation = 'Good';
    ELSEIF v_percentage >= 65 THEN
        SET v_gpa = 2.3;
        SET v_letter = 'C';
        SET v_evaluation = 'Good';
    ELSEIF v_percentage >= 60 THEN
        SET v_gpa = 2.0;
        SET v_letter = 'D+';
        SET v_evaluation = 'Pass';
    ELSEIF v_percentage >= 50 THEN
        SET v_gpa = 1.0;
        SET v_letter = 'D';
        SET v_evaluation = 'Pass';
    ELSE
        SET v_gpa = 0.0;
        SET v_letter = 'F';
        SET v_evaluation = 'Fail';
    END IF;
    
    -- Update grades table
    UPDATE grades 
    SET total = v_total,
        percentage = v_percentage,
        gpa = v_gpa,
        letter_grade = v_letter,
        evaluation = v_evaluation,
        updated_at = CURRENT_TIMESTAMP
    WHERE registration_id = p_registration_id;
END //

DELIMITER ;

-- ============================================
-- VIEW: Student Grades with Course Info
-- ============================================
CREATE OR REPLACE VIEW v_student_grades AS
SELECT 
    r.id AS registration_id,
    r.student_id,
    CONCAT(s.first_name, ' ', s.last_name) AS student_name,
    r.course_id,
    c.code AS course_code,
    c.name AS course_name,
    c.course_type,
    c.max_marks,
    c.credit_hours,
    c.assignment1_max,
    c.assignment2_max,
    c.coursework_max,
    c.final_exam_max,
    g.assignment1,
    g.assignment2,
    g.coursework,
    g.final_exam,
    g.total,
    g.percentage,
    g.gpa,
    g.letter_grade,
    g.evaluation,
    r.academic_year,
    r.semester,
    r.status AS registration_status
FROM registrations r
JOIN students s ON r.student_id = s.id
JOIN courses c ON r.course_id = c.id
LEFT JOIN grades g ON r.id = g.registration_id;

-- ============================================
-- VIEW: Professor's Assigned Courses
-- ============================================
CREATE OR REPLACE VIEW v_professor_courses AS
SELECT 
    ca.id AS assignment_id,
    ca.professor_id,
    CONCAT(p.first_name, ' ', p.last_name) AS professor_name,
    ca.course_id,
    c.code AS course_code,
    c.name AS course_name,
    ca.academic_year,
    ca.semester,
    (SELECT COUNT(*) FROM registrations r 
     WHERE r.course_id = ca.course_id 
     AND r.academic_year = ca.academic_year 
     AND r.semester = ca.semester
     AND r.status = 'Active') AS student_count
FROM course_assignments ca
JOIN professors p ON ca.professor_id = p.id
JOIN courses c ON ca.course_id = c.id;

-- ============================================
-- INSERT: Roles
-- ============================================
INSERT INTO roles (id, name, description) VALUES
('ROLE_SUPERADMIN', 'Super Admin', 'IT Administrator - Full system access'),
('ROLE_ADMIN', 'Student Affairs', 'Can manage students, courses, registrations'),
('ROLE_PROFESSOR', 'Professor', 'Can view assigned courses, enter grades, take attendance'),
('ROLE_STUDENT', 'Student', 'Read-only access to own data');

-- ============================================
-- INSERT: Permissions
-- ============================================
INSERT INTO permissions (id, name, description) VALUES
-- System
('PERM_SYS_SETTINGS', 'system.settings', 'Access system settings'),
('PERM_SYS_LOGS', 'system.logs', 'View audit logs'),
('PERM_SYS_BACKUP', 'system.backup', 'Backup/restore database'),
-- Users
('PERM_USER_CREATE', 'user.create', 'Create users'),
('PERM_USER_VIEW', 'user.view', 'View users'),
('PERM_USER_EDIT', 'user.edit', 'Edit users'),
('PERM_USER_DELETE', 'user.delete', 'Delete users'),
-- Students
('PERM_STU_CREATE', 'student.create', 'Add students'),
('PERM_STU_VIEW_ALL', 'student.view.all', 'View all students'),
('PERM_STU_VIEW_OWN', 'student.view.own', 'View own student profile'),
('PERM_STU_EDIT', 'student.edit', 'Edit students'),
('PERM_STU_DELETE', 'student.delete', 'Delete students'),
-- Courses
('PERM_CRS_CREATE', 'course.create', 'Add courses'),
('PERM_CRS_VIEW', 'course.view', 'View courses'),
('PERM_CRS_EDIT', 'course.edit', 'Edit courses'),
('PERM_CRS_DELETE', 'course.delete', 'Delete courses'),
('PERM_CRS_ASSIGN', 'course.assign', 'Assign professors to courses'),
-- Registrations
('PERM_REG_CREATE', 'registration.create', 'Register students to courses'),
('PERM_REG_VIEW', 'registration.view', 'View registrations'),
('PERM_REG_EDIT', 'registration.edit', 'Edit registrations'),
('PERM_REG_DELETE', 'registration.delete', 'Delete registrations'),
-- Grades
('PERM_GRD_VIEW_ALL', 'grade.view.all', 'View all grades'),
('PERM_GRD_VIEW_OWN', 'grade.view.own', 'View own grades'),
('PERM_GRD_VIEW_ASSIGNED', 'grade.view.assigned', 'View grades for assigned courses'),
('PERM_GRD_ENTER', 'grade.enter', 'Enter/edit grades'),
-- Attendance
('PERM_ATT_VIEW_ALL', 'attendance.view.all', 'View all attendance'),
('PERM_ATT_VIEW_OWN', 'attendance.view.own', 'View own attendance'),
('PERM_ATT_RECORD', 'attendance.record', 'Record attendance');

-- ============================================
-- INSERT: Role Permissions
-- ============================================
-- Super Admin - Full Access (except direct grade entry)
INSERT INTO role_permissions (role_id, permission_id) VALUES
('ROLE_SUPERADMIN', 'PERM_SYS_SETTINGS'),
('ROLE_SUPERADMIN', 'PERM_SYS_LOGS'),
('ROLE_SUPERADMIN', 'PERM_SYS_BACKUP'),
('ROLE_SUPERADMIN', 'PERM_USER_CREATE'),
('ROLE_SUPERADMIN', 'PERM_USER_VIEW'),
('ROLE_SUPERADMIN', 'PERM_USER_EDIT'),
('ROLE_SUPERADMIN', 'PERM_USER_DELETE'),
('ROLE_SUPERADMIN', 'PERM_STU_VIEW_ALL'),
('ROLE_SUPERADMIN', 'PERM_CRS_VIEW'),
('ROLE_SUPERADMIN', 'PERM_REG_VIEW'),
('ROLE_SUPERADMIN', 'PERM_GRD_VIEW_ALL'),
('ROLE_SUPERADMIN', 'PERM_ATT_VIEW_ALL');

-- Student Affairs Admin
INSERT INTO role_permissions (role_id, permission_id) VALUES
('ROLE_ADMIN', 'PERM_STU_CREATE'),
('ROLE_ADMIN', 'PERM_STU_VIEW_ALL'),
('ROLE_ADMIN', 'PERM_STU_EDIT'),
('ROLE_ADMIN', 'PERM_STU_DELETE'),
('ROLE_ADMIN', 'PERM_CRS_CREATE'),
('ROLE_ADMIN', 'PERM_CRS_VIEW'),
('ROLE_ADMIN', 'PERM_CRS_EDIT'),
('ROLE_ADMIN', 'PERM_CRS_DELETE'),
('ROLE_ADMIN', 'PERM_CRS_ASSIGN'),
('ROLE_ADMIN', 'PERM_REG_CREATE'),
('ROLE_ADMIN', 'PERM_REG_VIEW'),
('ROLE_ADMIN', 'PERM_REG_EDIT'),
('ROLE_ADMIN', 'PERM_REG_DELETE'),
('ROLE_ADMIN', 'PERM_GRD_VIEW_ALL'),
('ROLE_ADMIN', 'PERM_ATT_VIEW_ALL');

-- Professor
INSERT INTO role_permissions (role_id, permission_id) VALUES
('ROLE_PROFESSOR', 'PERM_CRS_VIEW'),
('ROLE_PROFESSOR', 'PERM_GRD_VIEW_ASSIGNED'),
('ROLE_PROFESSOR', 'PERM_GRD_ENTER'),
('ROLE_PROFESSOR', 'PERM_ATT_RECORD');

-- Student
INSERT INTO role_permissions (role_id, permission_id) VALUES
('ROLE_STUDENT', 'PERM_STU_VIEW_OWN'),
('ROLE_STUDENT', 'PERM_GRD_VIEW_OWN'),
('ROLE_STUDENT', 'PERM_ATT_VIEW_OWN');

-- ============================================
-- INSERT: Academic Levels
-- ============================================
INSERT INTO academic_levels (id, name, year, description) VALUES
('LEVEL1', 'First Year', 1, 'Freshman - Preparatory Year'),
('LEVEL2', 'Second Year', 2, 'Sophomore'),
('LEVEL3', 'Third Year', 3, 'Junior'),
('LEVEL4', 'Fourth Year', 4, 'Senior'),
('LEVEL5', 'Fifth Year', 5, 'Extended Study');

-- ============================================
-- INSERT: Colleges (NCTU Real Data)
-- ============================================
INSERT INTO colleges (id, name, name_ar, description) VALUES
('COL_IET', 'College of Industry & Energy Technology', '???? ????????? ??????? ???????', 'Main college with 5 departments'),
('COL_PROS', 'College of Prosthetics', '???? ??????? ????????', 'Specialized prosthetics program');

-- ============================================
-- INSERT: Departments (NCTU Real Data)
-- ============================================
INSERT INTO departments (id, name, name_ar, college_id, description) VALUES
-- College of Industry & Energy Technology
('DEPT_NRE', 'New & Renewable Energy Technology', '????????? ?????? ??????? ?????????', 'COL_IET', 'Solar, Wind, Hydro Power Systems'),
('DEPT_MECH', 'Mechatronics Technology', '????????? ????????????', 'COL_IET', 'Robotics, Automation, PLC'),
('DEPT_IT', 'Information Technology', '????????? ?????????', 'COL_IET', 'Software, Networks, Cybersecurity'),
('DEPT_AUTO', 'Autotronics Technology', '????????? ????????', 'COL_IET', 'Automotive Electronics & Diagnostics'),
('DEPT_PET', 'Petroleum Technology', '????????? ???????', 'COL_IET', 'Oil & Gas Processing'),
-- College of Prosthetics
('DEPT_PROS', 'Prosthetics & Orthotics', '??????? ???????? ???????? ?????????', 'COL_PROS', 'Prosthetic limbs and orthotic devices');

-- ============================================
-- INSERT: Default Users
-- ============================================
INSERT INTO users (id, username, password_hash, full_name, email, role_id) VALUES
('USR_SUPER', 'superadmin', 'admin123', 'IT Administrator', 'it@nctu.edu.eg', 'ROLE_SUPERADMIN'),
('USR_ADMIN', 'admin', 'admin123', 'Student Affairs Office', 'affairs@nctu.edu.eg', 'ROLE_ADMIN');

-- ============================================
-- INSERT: Sample Professors
-- ============================================
INSERT INTO users (id, username, password_hash, full_name, email, role_id) VALUES
('USR_PROF1', 'PROF001', 'prof123', 'Dr. Mohamed Ibrahim', 'mohamed.ibrahim@nctu.edu.eg', 'ROLE_PROFESSOR'),
('USR_PROF2', 'PROF002', 'prof123', 'Dr. Ahmed Hassan', 'ahmed.hassan@nctu.edu.eg', 'ROLE_PROFESSOR'),
('USR_PROF3', 'PROF003', 'prof123', 'Dr. Sara Ali', 'sara.ali@nctu.edu.eg', 'ROLE_PROFESSOR');

INSERT INTO professors (id, user_id, first_name, last_name, department_id, specialization, academic_title) VALUES
('PROF001', 'USR_PROF1', 'Mohamed', 'Ibrahim', 'DEPT_IT', 'Cybersecurity', 'Associate Professor'),
('PROF002', 'USR_PROF2', 'Ahmed', 'Hassan', 'DEPT_NRE', 'Solar Energy', 'Professor'),
('PROF003', 'USR_PROF3', 'Sara', 'Ali', 'DEPT_MECH', 'Robotics', 'Assistant Professor');

-- ============================================
-- INSERT: Sample Students
-- ============================================
INSERT INTO users (id, username, password_hash, full_name, email, role_id) VALUES
('USR_STU1', '20241', 'student123', 'Ahmed Mohamed Ali', 'ahmed.ali@student.nctu.edu.eg', 'ROLE_STUDENT'),
('USR_STU2', '20242', 'student123', 'Yousef Ibrahim Hassan', 'yousef.hassan@student.nctu.edu.eg', 'ROLE_STUDENT');

INSERT INTO students (id, user_id, first_name, last_name, gender, academic_level_id, department_id) VALUES
('20241', 'USR_STU1', 'Ahmed', 'Ali', 'Male', 'LEVEL1', 'DEPT_IT'),
('20242', 'USR_STU2', 'Yousef', 'Hassan', 'Male', 'LEVEL1', 'DEPT_IT');

-- ============================================
-- INSERT: Sample Courses (IT Department)
-- ============================================
INSERT INTO courses (id, code, name, name_ar, department_id, academic_level_id, course_type, max_marks, credit_hours, assignment1_max, assignment2_max, coursework_max, final_exam_max) VALUES
-- First Year IT Courses
('CRS_IT101', 'IT101', 'Introduction to Programming', '????? ?? ???????', 'DEPT_IT', 'LEVEL1', 'mixed', 100, 3, 10, 10, 20, 60),
('CRS_IT102', 'IT102', 'Computer Fundamentals', '??????? ??????', 'DEPT_IT', 'LEVEL1', 'theoretical', 100, 3, 10, 10, 20, 60),
('CRS_IT103', 'IT103', 'Cybersecurity Basics', '??????? ????? ?????????', 'DEPT_IT', 'LEVEL1', 'theoretical', 100, 3, 10, 10, 20, 60),
('CRS_IT104', 'IT104', 'Networking Fundamentals', '??????? ???????', 'DEPT_IT', 'LEVEL1', 'mixed', 150, 4, 15, 15, 30, 90),
('CRS_IT105', 'IT105', 'Database Systems', '??? ????? ????????', 'DEPT_IT', 'LEVEL2', 'mixed', 100, 3, 10, 10, 20, 60),
('CRS_IT106', 'IT106', 'Web Development', '????? ?????', 'DEPT_IT', 'LEVEL2', 'practical', 150, 4, 20, 20, 30, 80);

-- ============================================
-- INSERT: Course Assignments
-- ============================================
INSERT INTO course_assignments (id, professor_id, course_id, academic_year, semester) VALUES
('CA001', 'PROF001', 'CRS_IT101', '2024-2025', 'Fall'),
('CA002', 'PROF001', 'CRS_IT103', '2024-2025', 'Fall'),
('CA003', 'PROF002', 'CRS_IT102', '2024-2025', 'Fall');

-- ============================================
-- INSERT: Lecture Halls
-- ============================================
INSERT INTO lecture_halls (id, name, building, floor, max_capacity) VALUES
('HALL_A1', 'Hall A1', 'Main Building', 1, 100),
('HALL_A2', 'Hall A2', 'Main Building', 1, 80),
('HALL_B1', 'Hall B1', 'IT Building', 2, 60);

-- ============================================
-- INSERT: Laboratories
-- ============================================
INSERT INTO laboratories (id, name, building, floor, max_capacity, lab_type, equipment_count) VALUES
('LAB_IT1', 'Computer Lab 1', 'IT Building', 1, 30, 'Computer', 30),
('LAB_IT2', 'Computer Lab 2', 'IT Building', 1, 25, 'Computer', 25),
('LAB_NET', 'Networking Lab', 'IT Building', 2, 20, 'Computer', 20);

-- ============================================
-- INSERT: Active Semester
-- ============================================
INSERT INTO semesters (id, name, academic_year, semester_type, start_date, end_date, is_active) VALUES
('SEM_2024F', 'Fall 2024-2025', '2024-2025', 'Fall', '2024-09-15', '2025-01-15', TRUE);

SELECT 'Schema v2.0 created successfully!' AS status;
