-- ================================================================================
-- NCTU Student Information System - Schema v3 (RBAC + Curriculum)
-- جامعة القاهرة الجديدة التكنولوجية
-- ================================================================================

DROP DATABASE IF EXISTS nctu_sis;

CREATE DATABASE nctu_sis
CHARACTER SET utf8mb4
COLLATE utf8mb4_unicode_ci;

USE nctu_sis;

-- Drop existing tables (safe order)
DROP TABLE IF EXISTS user_permissions;
DROP TABLE IF EXISTS role_permissions;
DROP TABLE IF EXISTS permissions;
DROP TABLE IF EXISTS audit_log;
DROP TABLE IF EXISTS attendance;
DROP TABLE IF EXISTS sessions;
DROP TABLE IF EXISTS grades;
DROP TABLE IF EXISTS registrations;
DROP TABLE IF EXISTS course_assignments;
DROP TABLE IF EXISTS courses;
DROP TABLE IF EXISTS course_types;
DROP TABLE IF EXISTS professors;
DROP TABLE IF EXISTS students;
DROP TABLE IF EXISTS users;
DROP TABLE IF EXISTS roles;
DROP TABLE IF EXISTS semesters;
DROP TABLE IF EXISTS academic_levels;
DROP TABLE IF EXISTS departments;
DROP TABLE IF EXISTS colleges;
DROP TABLE IF EXISTS lecture_halls;
DROP TABLE IF EXISTS laboratories;

-- =============================================================
-- RBAC TABLES
-- =============================================================
CREATE TABLE roles (
    id VARCHAR(30) PRIMARY KEY,
    name_en VARCHAR(100) NOT NULL,
    name_ar VARCHAR(100),
    description TEXT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE permissions (
    id VARCHAR(30) PRIMARY KEY,
    name VARCHAR(100) NOT NULL UNIQUE,
    description TEXT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE role_permissions (
    role_id VARCHAR(30) NOT NULL,
    permission_id VARCHAR(30) NOT NULL,
    PRIMARY KEY (role_id, permission_id),
    FOREIGN KEY (role_id) REFERENCES roles(id) ON DELETE CASCADE,
    FOREIGN KEY (permission_id) REFERENCES permissions(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE user_permissions (
    user_id INT NOT NULL,
    permission_id VARCHAR(30) NOT NULL,
    is_granted BOOLEAN NOT NULL,
    PRIMARY KEY (user_id, permission_id),
    FOREIGN KEY (permission_id) REFERENCES permissions(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- =============================================================
-- CORE MASTER DATA
-- =============================================================
CREATE TABLE colleges (
    id VARCHAR(10) PRIMARY KEY,
    name_ar VARCHAR(200) NOT NULL,
    name_en VARCHAR(200) NOT NULL,
    is_active BOOLEAN DEFAULT TRUE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE departments (
    id VARCHAR(10) PRIMARY KEY,
    college_id VARCHAR(10) NOT NULL,
    name_ar VARCHAR(200) NOT NULL,
    name_en VARCHAR(200) NOT NULL,
    code VARCHAR(10) NOT NULL,
    FOREIGN KEY (college_id) REFERENCES colleges(id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE academic_levels (
    id VARCHAR(10) PRIMARY KEY,
    name_ar VARCHAR(100) NOT NULL,
    name_en VARCHAR(100) NOT NULL,
    year_number INT NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE semesters (
    id VARCHAR(20) PRIMARY KEY,
    name_ar VARCHAR(100),
    name_en VARCHAR(100),
    academic_year VARCHAR(20),
    semester_number INT,
    is_current BOOLEAN DEFAULT FALSE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE course_types (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    max_marks INT NOT NULL,
    assignment1_max INT DEFAULT 0,
    assignment2_max INT DEFAULT 0,
    year_work_max INT DEFAULT 0,
    final_exam_max INT DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE courses (
    id VARCHAR(30) PRIMARY KEY,
    code VARCHAR(20) NOT NULL,
    name_ar VARCHAR(200),
    name_en VARCHAR(200) NOT NULL,
    description TEXT,
    department_id VARCHAR(10) NOT NULL,
    academic_level_id VARCHAR(10) NOT NULL,
    semester_number INT NOT NULL,
    course_type_id VARCHAR(20) NOT NULL,
    credit_hours INT NOT NULL,
    lecture_hours INT DEFAULT 0,
    tutorial_hours INT DEFAULT 0,
    lab_hours INT DEFAULT 0,
    practical_hours INT DEFAULT 0,
    max_marks INT NOT NULL,
    assignment1_marks INT DEFAULT 0,
    assignment2_marks INT DEFAULT 0,
    year_work_marks INT DEFAULT 0,
    final_exam_marks INT DEFAULT 0,
    FOREIGN KEY (department_id) REFERENCES departments(id),
    FOREIGN KEY (academic_level_id) REFERENCES academic_levels(id),
    FOREIGN KEY (course_type_id) REFERENCES course_types(id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- =============================================================
-- USERS, STUDENTS, PROFESSORS
-- =============================================================
CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    full_name VARCHAR(150) NOT NULL,
    email VARCHAR(200),
    phone VARCHAR(20),
    role_id VARCHAR(30) NOT NULL,
    is_active BOOLEAN DEFAULT TRUE,
    last_login TIMESTAMP NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (role_id) REFERENCES roles(id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE students (
    id VARCHAR(20) PRIMARY KEY,
    user_id INT,
    first_name VARCHAR(100) NOT NULL,
    last_name VARCHAR(100) NOT NULL,
    date_of_birth DATE,
    email VARCHAR(200),
    phone VARCHAR(20),
    gender ENUM('male', 'female') NOT NULL,
    department_id VARCHAR(10) NOT NULL,
    academic_level_id VARCHAR(10) NOT NULL,
    enrollment_date DATE DEFAULT (CURRENT_DATE),
    status ENUM('active', 'inactive', 'graduated') DEFAULT 'active',
    FOREIGN KEY (user_id) REFERENCES users(id),
    FOREIGN KEY (department_id) REFERENCES departments(id),
    FOREIGN KEY (academic_level_id) REFERENCES academic_levels(id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE professors (
    id VARCHAR(20) PRIMARY KEY,
    user_id INT,
    title VARCHAR(20),
    first_name VARCHAR(100) NOT NULL,
    last_name VARCHAR(100) NOT NULL,
    email VARCHAR(200),
    phone VARCHAR(20),
    department_id VARCHAR(10) NOT NULL,
    hire_date DATE,
    specialization VARCHAR(120),
    FOREIGN KEY (user_id) REFERENCES users(id),
    FOREIGN KEY (department_id) REFERENCES departments(id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- =============================================================
-- ASSIGNMENTS, REGISTRATIONS, GRADES
-- =============================================================
CREATE TABLE course_assignments (
    id INT AUTO_INCREMENT PRIMARY KEY,
    course_id VARCHAR(30) NOT NULL,
    professor_id VARCHAR(20) NOT NULL,
    semester_id VARCHAR(20) NOT NULL,
    is_primary BOOLEAN DEFAULT TRUE,
    assigned_by INT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE KEY unique_assignment (course_id, professor_id, semester_id),
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE,
    FOREIGN KEY (professor_id) REFERENCES professors(id) ON DELETE CASCADE,
    FOREIGN KEY (semester_id) REFERENCES semesters(id) ON DELETE CASCADE,
    FOREIGN KEY (assigned_by) REFERENCES users(id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE registrations (
    id INT AUTO_INCREMENT PRIMARY KEY,
    student_id VARCHAR(20) NOT NULL,
    course_id VARCHAR(30) NOT NULL,
    semester_id VARCHAR(20) NOT NULL,
    status ENUM('registered', 'dropped', 'completed') DEFAULT 'registered',
    registered_by INT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (student_id) REFERENCES students(id) ON DELETE CASCADE,
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE,
    FOREIGN KEY (semester_id) REFERENCES semesters(id) ON DELETE CASCADE,
    FOREIGN KEY (registered_by) REFERENCES users(id) ON DELETE SET NULL,
    UNIQUE KEY unique_reg (student_id, course_id, semester_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE grades (
    id INT AUTO_INCREMENT PRIMARY KEY,
    registration_id INT UNIQUE NOT NULL,
    assignment1 DECIMAL(5,2) DEFAULT 0,
    assignment2 DECIMAL(5,2) DEFAULT 0,
    year_work DECIMAL(5,2) DEFAULT 0,
    final_exam DECIMAL(5,2) DEFAULT 0,
    total_marks DECIMAL(5,2) DEFAULT 0,
    percentage DECIMAL(5,2) DEFAULT 0,
    gpa DECIMAL(3,2) DEFAULT 0,
    evaluation ENUM('Excellent', 'Very Good', 'Good', 'Pass', 'Fail') DEFAULT 'Fail',
    letter_grade VARCHAR(2) DEFAULT 'F',
    last_modified_by INT,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (registration_id) REFERENCES registrations(id) ON DELETE CASCADE,
    FOREIGN KEY (last_modified_by) REFERENCES users(id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- =============================================================
-- SESSIONS & ATTENDANCE
-- =============================================================
CREATE TABLE sessions (
    id INT AUTO_INCREMENT PRIMARY KEY,
    course_id VARCHAR(30) NOT NULL,
    professor_id VARCHAR(20),
    semester_id VARCHAR(20) NOT NULL,
    session_date DATE NOT NULL,
    session_type ENUM('lecture', 'lab', 'tutorial') NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (course_id) REFERENCES courses(id),
    FOREIGN KEY (professor_id) REFERENCES professors(id),
    FOREIGN KEY (semester_id) REFERENCES semesters(id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE attendance (
    id INT AUTO_INCREMENT PRIMARY KEY,
    session_id INT NOT NULL,
    student_id VARCHAR(20) NOT NULL,
    status ENUM('present', 'absent', 'late', 'excused') NOT NULL,
    remarks TEXT,
    recorded_by INT,
    recorded_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (session_id) REFERENCES sessions(id) ON DELETE CASCADE,
    FOREIGN KEY (student_id) REFERENCES students(id) ON DELETE CASCADE,
    FOREIGN KEY (recorded_by) REFERENCES users(id) ON DELETE SET NULL,
    UNIQUE KEY unique_att (session_id, student_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- =============================================================
-- OPTIONAL FACILITIES TABLES
-- =============================================================
CREATE TABLE lecture_halls (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
    building VARCHAR(50),
    floor INT,
    max_capacity INT NOT NULL,
    seating_desc VARCHAR(200),
    ac_units INT DEFAULT 0,
    fan_units INT DEFAULT 0,
    lighting_units INT DEFAULT 0,
    description TEXT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE laboratories (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
    building VARCHAR(50),
    floor INT,
    max_capacity INT NOT NULL,
    lab_type VARCHAR(50),
    computers_count INT DEFAULT 0,
    seats_count INT DEFAULT 0,
    ac_units INT DEFAULT 0,
    fan_units INT DEFAULT 0,
    lighting_units INT DEFAULT 0,
    description TEXT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- =============================================================
-- AUDIT LOG
-- =============================================================
CREATE TABLE audit_log (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    user_id INT,
    action VARCHAR(50) NOT NULL,
    table_name VARCHAR(50),
    record_id VARCHAR(50),
    details JSON,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- =============================================================
-- DEFAULT ROLES & PERMISSIONS
-- =============================================================
INSERT INTO roles (id, name_en, name_ar, description) VALUES
('ROLE_SUPERADMIN', 'Super Admin', 'مسؤول النظام', 'Full system access'),
('ROLE_STUDENT_AFFAIRS', 'Student Affairs', 'شؤون الطلاب', 'Manage students & registrations'),
('ROLE_PROFESSOR', 'Professor', 'عضو هيئة تدريس', 'Enter grades & attendance for assigned courses'),
('ROLE_STUDENT', 'Student', 'طالب', 'View own data');

INSERT INTO permissions (id, name, description) VALUES
('PERM_USER_MANAGE', 'user.manage', 'Manage users & permissions'),
('PERM_STU_CREATE', 'student.create', 'Add students'),
('PERM_STU_VIEW', 'student.view', 'View students'),
('PERM_STU_EDIT', 'student.edit', 'Edit students'),
('PERM_STU_DELETE', 'student.delete', 'Delete students'),
('PERM_PROF_CREATE', 'professor.create', 'Add professors'),
('PERM_PROF_VIEW', 'professor.view', 'View professors'),
('PERM_PROF_EDIT', 'professor.edit', 'Edit professors'),
('PERM_PROF_DELETE', 'professor.delete', 'Delete professors'),
('PERM_CRS_CREATE', 'course.create', 'Add courses'),
('PERM_CRS_VIEW', 'course.view', 'View courses'),
('PERM_CRS_EDIT', 'course.edit', 'Edit courses'),
('PERM_CRS_DELETE', 'course.delete', 'Delete courses'),
('PERM_CRS_ASSIGN', 'course.assign', 'Assign professors to courses'),
('PERM_REG_CREATE', 'registration.create', 'Register students to courses'),
('PERM_REG_VIEW', 'registration.view', 'View registrations'),
('PERM_REG_EDIT', 'registration.edit', 'Edit registrations'),
('PERM_REG_DELETE', 'registration.delete', 'Delete registrations'),
('PERM_GRD_VIEW_ALL', 'grade.view.all', 'View all grades'),
('PERM_GRD_VIEW_ASSIGNED', 'grade.view.assigned', 'View assigned grades'),
('PERM_GRD_VIEW_OWN', 'grade.view.own', 'View own grades'),
('PERM_GRD_ENTER', 'grade.enter', 'Enter grades'),
('PERM_ATT_VIEW_ALL', 'attendance.view.all', 'View attendance'),
('PERM_ATT_VIEW_OWN', 'attendance.view.own', 'View own attendance'),
('PERM_ATT_RECORD', 'attendance.record', 'Record attendance'),
('PERM_PWD_VIEW_ALL', 'password.view.all', 'View all user passwords'),
('PERM_PWD_VIEW_STU', 'password.view.student', 'View student passwords only'),
('PERM_ROLE_MANAGE', 'role.manage', 'Create/edit/delete roles');

-- Super Admin: full access
INSERT INTO role_permissions (role_id, permission_id)
SELECT 'ROLE_SUPERADMIN', id FROM permissions;

-- Student Affairs permissions (removed professor management - only manages students)
INSERT INTO role_permissions (role_id, permission_id) VALUES
('ROLE_STUDENT_AFFAIRS', 'PERM_STU_CREATE'),
('ROLE_STUDENT_AFFAIRS', 'PERM_STU_VIEW'),
('ROLE_STUDENT_AFFAIRS', 'PERM_STU_EDIT'),
('ROLE_STUDENT_AFFAIRS', 'PERM_STU_DELETE'),
('ROLE_STUDENT_AFFAIRS', 'PERM_CRS_CREATE'),
('ROLE_STUDENT_AFFAIRS', 'PERM_CRS_VIEW'),
('ROLE_STUDENT_AFFAIRS', 'PERM_CRS_EDIT'),
('ROLE_STUDENT_AFFAIRS', 'PERM_CRS_DELETE'),
('ROLE_STUDENT_AFFAIRS', 'PERM_CRS_ASSIGN'),
('ROLE_STUDENT_AFFAIRS', 'PERM_REG_CREATE'),
('ROLE_STUDENT_AFFAIRS', 'PERM_REG_VIEW'),
('ROLE_STUDENT_AFFAIRS', 'PERM_REG_EDIT'),
('ROLE_STUDENT_AFFAIRS', 'PERM_REG_DELETE'),
('ROLE_STUDENT_AFFAIRS', 'PERM_GRD_VIEW_ALL'),
('ROLE_STUDENT_AFFAIRS', 'PERM_ATT_VIEW_ALL'),
('ROLE_STUDENT_AFFAIRS', 'PERM_PWD_VIEW_STU');

-- Professor permissions
INSERT INTO role_permissions (role_id, permission_id) VALUES
('ROLE_PROFESSOR', 'PERM_CRS_VIEW'),
('ROLE_PROFESSOR', 'PERM_GRD_VIEW_ASSIGNED'),
('ROLE_PROFESSOR', 'PERM_GRD_ENTER'),
('ROLE_PROFESSOR', 'PERM_ATT_RECORD');

-- Student permissions
INSERT INTO role_permissions (role_id, permission_id) VALUES
('ROLE_STUDENT', 'PERM_GRD_VIEW_OWN'),
('ROLE_STUDENT', 'PERM_ATT_VIEW_OWN');

-- =============================================================
-- LOOKUP DATA (from provided documents)
-- =============================================================
-- Course types with new grade distribution (Assignment1, Assignment2, YearWork, Final)
-- For 100 marks: 20 + 20 + 20 + 40 = 100
-- For 150 marks: 30 + 45 + 30 + 45 = 150
INSERT INTO course_types (id, name, max_marks, assignment1_max, assignment2_max, year_work_max, final_exam_max) VALUES
('THEO_50', 'Theoretical 50 marks', 50, 10, 10, 10, 20),
('THEO_100', 'Theoretical 100 marks', 100, 20, 20, 20, 40),
('THEO_150', 'Theoretical 150 marks', 150, 30, 45, 30, 45),
('PRAC_100', 'Practical 100 marks', 100, 20, 20, 20, 40),
('PRAC_150', 'Practical 150 marks', 150, 30, 45, 30, 45),
('PRAC_200', 'Practical 200 marks', 200, 40, 60, 40, 60),
('PRAC_250', 'Practical 250 marks', 250, 50, 75, 50, 75),
('PROJECT_150', 'Project 150 marks', 150, 30, 45, 30, 45);

INSERT INTO colleges (id, name_ar, name_en) VALUES
('COL001', 'كلية تكنولوجيا الصناعة والطاقة', 'College of Industry and Energy Technology'),
('COL002', 'كلية الأطراف الصناعية', 'College of Prosthetics');

INSERT INTO departments (id, college_id, name_ar, name_en, code) VALUES
('DEPT001', 'COL001', 'قسم الطاقة الجديدة والمتجددة', 'New and Renewable Energy', 'REN'),
('DEPT002', 'COL001', 'قسم الميكاترونيكس', 'Mechatronics', 'MEC'),
('DEPT003', 'COL001', 'قسم تكنولوجيا المعلومات', 'Information Technology', 'IT'),
('DEPT004', 'COL001', 'قسم الأوتوترونيكس', 'Autotronics', 'AUT'),
('DEPT005', 'COL001', 'قسم تكنولوجيا البترول', 'Petroleum Technology', 'PET'),
('DEPT006', 'COL002', 'قسم الأطراف الصناعية', 'Prosthetics', 'PRO');

INSERT INTO academic_levels (id, name_ar, name_en, year_number) VALUES
('LEVEL1', 'الفرقة الأولى', 'First Year', 1),
('LEVEL2', 'الفرقة الثانية', 'Second Year', 2),
('LEVEL3', 'الفرقة الثالثة', 'Third Year', 3),
('LEVEL4', 'الفرقة الرابعة', 'Fourth Year', 4);

INSERT INTO semesters (id, name_ar, name_en, academic_year, semester_number, is_current) VALUES
('2024-1', 'الفصل الأول 2024-2025', 'Fall 2024', '2024-2025', 1, FALSE),
('2024-2', 'الفصل الثاني 2024-2025', 'Spring 2025', '2024-2025', 2, FALSE),
('2025-1', 'الفصل الأول 2025-2026', 'Fall 2025', '2025-2026', 1, TRUE),
('2025-2', 'الفصل الثاني 2025-2026', 'Spring 2026', '2025-2026', 2, FALSE);

-- Default users
INSERT INTO users (username, password_hash, full_name, role_id) VALUES
('superadmin', 'admin123', 'System Administrator', 'ROLE_SUPERADMIN'),
('affairs', 'affairs123', 'Student Affairs Office', 'ROLE_STUDENT_AFFAIRS');

-- ================================================================================
-- COURSES DATA (as provided)
-- ================================================================================

INSERT INTO courses (id, code, name_en, department_id, academic_level_id, semester_number, course_type_id, credit_hours, lecture_hours, tutorial_hours, lab_hours, max_marks, assignment1_marks, assignment2_marks, year_work_marks, final_exam_marks) VALUES
('IT-Y1S1-01', 'IT111', 'Intro to Cyber Security', 'DEPT003', 'LEVEL1', 1, 'THEO_100', 1, 1, 0, 0, 100, 20, 20, 20, 40),
('IT-Y1S1-02', 'IT112', 'IT Essentials', 'DEPT003', 'LEVEL1', 1, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y1S1-03', 'IT113', 'Technical English 1', 'DEPT003', 'LEVEL1', 1, 'THEO_100', 3, 2, 2, 0, 100, 20, 20, 20, 40),
('IT-Y1S1-04', 'IT114', 'Mathematics 1', 'DEPT003', 'LEVEL1', 1, 'THEO_100', 3, 2, 2, 0, 100, 20, 20, 20, 40),
('IT-Y1S1-05', 'IT115', 'Physics', 'DEPT003', 'LEVEL1', 1, 'THEO_100', 3, 2, 2, 0, 100, 20, 20, 20, 40),
('IT-Y1S1-06', 'IT116', 'Programming Essentials in Python', 'DEPT003', 'LEVEL1', 1, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y1S2-01', 'IT121', 'Programming Essentials in C', 'DEPT003', 'LEVEL1', 2, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y1S2-02', 'IT122', 'Cyber Security Essentials', 'DEPT003', 'LEVEL1', 2, 'THEO_100', 2, 2, 0, 0, 100, 20, 20, 20, 40),
('IT-Y1S2-03', 'IT123', 'Intro to IoT Connecting Things', 'DEPT003', 'LEVEL1', 2, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y1S2-04', 'IT124', 'MS Office', 'DEPT003', 'LEVEL1', 2, 'PRAC_100', 2, 1, 0, 4, 100, 20, 20, 20, 40),
('IT-Y1S2-05', 'IT125', 'Technical English 2', 'DEPT003', 'LEVEL1', 2, 'THEO_100', 3, 2, 2, 0, 100, 20, 20, 20, 40),
('IT-Y1S2-06', 'IT126', 'Mathematics 2', 'DEPT003', 'LEVEL1', 2, 'THEO_100', 3, 2, 2, 0, 100, 20, 20, 20, 40),
('IT-Y2S1-01', 'IT211', 'Linux Essentials', 'DEPT003', 'LEVEL2', 1, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y2S1-02', 'IT212', 'Programming Essentials in C++', 'DEPT003', 'LEVEL2', 1, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y2S1-03', 'IT213', 'Web Programming I', 'DEPT003', 'LEVEL2', 1, 'PRAC_100', 3, 1, 0, 6, 100, 20, 20, 20, 40),
('IT-Y2S1-04', 'IT214', 'Introduction to Database', 'DEPT003', 'LEVEL2', 1, 'THEO_100', 2, 2, 0, 0, 100, 20, 20, 20, 40),
('IT-Y2S1-05', 'IT215', 'Digital Engineering', 'DEPT003', 'LEVEL2', 1, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y2S1-06', 'IT216', 'Operating System', 'DEPT003', 'LEVEL2', 1, 'THEO_100', 2, 2, 0, 0, 100, 20, 20, 20, 40),
('IT-Y2S2-01', 'IT221', 'Java Programming I', 'DEPT003', 'LEVEL2', 2, 'PRAC_100', 3, 1, 0, 6, 100, 20, 20, 20, 40),
('IT-Y2S2-02', 'IT222', 'Web Programming II', 'DEPT003', 'LEVEL2', 2, 'PRAC_100', 3, 1, 0, 6, 100, 20, 20, 20, 40),
('IT-Y2S2-03', 'IT223', 'CCNA R&S I', 'DEPT003', 'LEVEL2', 2, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y2S2-04', 'IT224', 'Data Structure', 'DEPT003', 'LEVEL2', 2, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y2S2-05', 'IT225', 'Database Programming', 'DEPT003', 'LEVEL2', 2, 'PRAC_100', 2, 1, 0, 4, 100, 20, 20, 20, 40),
('IT-Y2S2-06', 'IT226', 'Capstone Design', 'DEPT003', 'LEVEL2', 2, 'PRAC_100', 2, 0, 0, 6, 100, 20, 20, 20, 40),
('IT-Y3S1-01', 'IT311', 'Advanced Programming in C', 'DEPT003', 'LEVEL3', 1, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y3S1-02', 'IT312', 'Data Communication', 'DEPT003', 'LEVEL3', 1, 'THEO_100', 2, 2, 0, 0, 100, 20, 20, 20, 40),
('IT-Y3S1-03', 'IT313', 'Java Programming II', 'DEPT003', 'LEVEL3', 1, 'PRAC_100', 3, 1, 0, 6, 100, 20, 20, 20, 40),
('IT-Y3S1-04', 'IT314', 'Computer Architecture', 'DEPT003', 'LEVEL3', 1, 'THEO_100', 2, 2, 0, 0, 100, 20, 20, 20, 40),
('IT-Y3S1-05', 'IT315', 'Microprocessor', 'DEPT003', 'LEVEL3', 1, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y3S1-06', 'IT316', 'Computer Graphics', 'DEPT003', 'LEVEL3', 1, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y3S2-01', 'IT321', 'Advanced Programming in C++', 'DEPT003', 'LEVEL3', 2, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y3S2-02', 'IT322', 'Mobile Programming I', 'DEPT003', 'LEVEL3', 2, 'PRAC_100', 3, 1, 0, 6, 100, 20, 20, 20, 40),
('IT-Y3S2-03', 'IT323', 'Embedded System', 'DEPT003', 'LEVEL3', 2, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y3S2-04', 'IT324', 'Network Programming', 'DEPT003', 'LEVEL3', 2, 'PRAC_100', 3, 2, 0, 4, 100, 20, 20, 20, 40),
('IT-Y3S2-05', 'IT325', 'Algorithm', 'DEPT003', 'LEVEL3', 2, 'THEO_100', 2, 2, 0, 0, 100, 20, 20, 20, 40),
('IT-Y3S2-06', 'IT326', 'Software Engineering', 'DEPT003', 'LEVEL3', 2, 'THEO_100', 2, 2, 0, 0, 100, 20, 20, 20, 40),
('IT-Y4S1-01', 'IT411', 'CCNA R&S II', 'DEPT003', 'LEVEL4', 1, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y4S1-02', 'IT412', 'Mobile Programming II', 'DEPT003', 'LEVEL4', 1, 'PRAC_100', 3, 1, 0, 6, 100, 20, 20, 20, 40),
('IT-Y4S1-03', 'IT413', 'IoT Architecture & Protocols', 'DEPT003', 'LEVEL4', 1, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y4S1-04', 'IT414', 'Artificial Intelligence', 'DEPT003', 'LEVEL4', 1, 'THEO_100', 2, 2, 0, 0, 100, 20, 20, 20, 40),
('IT-Y4S1-05', 'IT415', 'Windows Programming I', 'DEPT003', 'LEVEL4', 1, 'PRAC_100', 3, 1, 0, 6, 100, 20, 20, 20, 40),
('IT-Y4S1-06', 'IT416', 'Signal Processing', 'DEPT003', 'LEVEL4', 1, 'THEO_100', 2, 2, 0, 0, 100, 20, 20, 20, 40),
('IT-Y4S2-01', 'IT421', 'Robotics', 'DEPT003', 'LEVEL4', 2, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y4S2-02', 'IT422', 'Windows Programming II', 'DEPT003', 'LEVEL4', 2, 'PRAC_100', 3, 1, 0, 6, 100, 20, 20, 20, 40),
('IT-Y4S2-03', 'IT423', 'Big Data Analytics', 'DEPT003', 'LEVEL4', 2, 'PRAC_100', 2, 1, 0, 3, 100, 20, 20, 20, 40),
('IT-Y4S2-04', 'IT424', 'IoT Security', 'DEPT003', 'LEVEL4', 2, 'PRAC_100', 3, 2, 0, 3, 100, 20, 20, 20, 40),
('IT-Y4S2-05', 'IT425', 'Machine Learning', 'DEPT003', 'LEVEL4', 2, 'THEO_100', 2, 2, 0, 0, 100, 20, 20, 20, 40),
('IT-Y4S2-06', 'IT426', 'Entrepreneurship', 'DEPT003', 'LEVEL4', 2, 'THEO_50', 1, 1, 0, 0, 50, 10, 10, 10, 20),
('IT-Y4S2-07', 'IT427', 'Graduation Project', 'DEPT003', 'LEVEL4', 2, 'PROJECT_150', 2, 0, 0, 6, 150, 30, 30, 45, 45);


-- ================================================================================
-- PART 4: MECHATRONICS COURSES (33 courses)
-- ================================================================================

INSERT INTO courses (id, code, name_en, department_id, academic_level_id, semester_number, course_type_id, credit_hours, lecture_hours, tutorial_hours, lab_hours, max_marks, assignment1_marks, assignment2_marks, year_work_marks, final_exam_marks) VALUES
('MEC-Y1S1-01', 'MEC111', 'Technical English at Work Place I', 'DEPT002', 'LEVEL1', 1, 'THEO_150', 3, 2, 2, 0, 150, 30, 45, 30, 45),
('MEC-Y1S1-02', 'MEC112', 'Communication Skills in Technology', 'DEPT002', 'LEVEL1', 1, 'PRAC_150', 3, 1, 2, 3, 150, 30, 45, 30, 45),
('MEC-Y1S1-03', 'MEC113', 'Mathematics for Technicians', 'DEPT002', 'LEVEL1', 1, 'THEO_150', 3, 2, 2, 0, 150, 30, 45, 30, 45),
('MEC-Y1S1-04', 'MEC114', 'Application of Math and Science', 'DEPT002', 'LEVEL1', 1, 'THEO_150', 3, 2, 2, 0, 150, 30, 45, 30, 45),
('MEC-Y1S1-05', 'MEC115', 'Health Safety and Risk Assessment', 'DEPT002', 'LEVEL1', 1, 'PRAC_100', 2, 2, 0, 4, 100, 20, 20, 20, 40),
('MEC-Y1S1-06', 'MEC116', 'Physics for Technicians', 'DEPT002', 'LEVEL1', 1, 'THEO_100', 2, 1, 2, 0, 100, 20, 20, 20, 40),
('MEC-Y1S2-01', 'MEC121', 'Technical English at Work Place II', 'DEPT002', 'LEVEL1', 2, 'THEO_150', 3, 2, 2, 0, 150, 30, 45, 30, 45),
('MEC-Y1S2-02', 'MEC122', 'Environmental Studies', 'DEPT002', 'LEVEL1', 2, 'THEO_50', 1, 1, 0, 0, 50, 10, 10, 10, 20),
('MEC-Y1S2-03', 'MEC123', 'Basic Mechatronics Workshop', 'DEPT002', 'LEVEL1', 2, 'PRAC_150', 3, 2, 0, 4, 150, 30, 45, 30, 45),
('MEC-Y1S2-04', 'MEC124', 'Computer Technology', 'DEPT002', 'LEVEL1', 2, 'PRAC_100', 2, 1, 0, 3, 100, 20, 20, 20, 40),
('MEC-Y1S2-05', 'MEC125', 'Electric Circuits', 'DEPT002', 'LEVEL1', 2, 'PRAC_150', 3, 2, 0, 4, 150, 30, 45, 30, 45),
('MEC-Y1S2-06', 'MEC126', 'CAD for Technicians', 'DEPT002', 'LEVEL1', 2, 'PRAC_100', 1, 0, 0, 4, 100, 20, 20, 20, 40),
('MEC-Y1S2-07', 'MEC127', 'Computer-Aided Kinematics', 'DEPT002', 'LEVEL1', 2, 'PRAC_150', 3, 2, 0, 3, 150, 30, 45, 30, 45),
('MEC-Y2S1-01', 'MEC211', 'Pneumatics and Hydraulics Practice', 'DEPT002', 'LEVEL2', 1, 'PRAC_150', 3, 2, 0, 4, 150, 30, 45, 30, 45),
('MEC-Y2S1-02', 'MEC212', 'Programming for Mechatronics', 'DEPT002', 'LEVEL2', 1, 'PRAC_150', 3, 2, 0, 4, 150, 30, 45, 30, 45),
('MEC-Y2S1-03', 'MEC213', 'Material Selection', 'DEPT002', 'LEVEL2', 1, 'PRAC_150', 3, 2, 0, 4, 150, 30, 45, 30, 45),
('MEC-Y2S1-04', 'MEC214', 'Fundamentals of PC-based Control', 'DEPT002', 'LEVEL2', 1, 'PRAC_150', 3, 2, 0, 4, 150, 30, 45, 30, 45),
('MEC-Y2S1-05', 'MEC215', 'PLC', 'DEPT002', 'LEVEL2', 1, 'PRAC_150', 4, 2, 2, 3, 150, 30, 45, 30, 45),
('MEC-Y2S2-01', 'MEC221', 'Electrical and Electronics Workshop', 'DEPT002', 'LEVEL2', 2, 'PRAC_100', 1, 0, 0, 4, 100, 20, 20, 20, 40),
('MEC-Y2S2-02', 'MEC222', 'Electromechanical Systems Maintenance', 'DEPT002', 'LEVEL2', 2, 'PRAC_150', 3, 2, 0, 4, 150, 30, 45, 30, 45),
('MEC-Y2S2-03', 'MEC223', 'Capstone Design', 'DEPT002', 'LEVEL2', 2, 'PRAC_150', 3, 1, 0, 6, 150, 30, 45, 30, 45),
('MEC-Y2S2-04', 'MEC224', 'Mechatronic Systems for Technologists', 'DEPT002', 'LEVEL2', 2, 'PRAC_150', 3, 2, 0, 4, 150, 30, 45, 30, 45),
('MEC-Y2S2-05', 'MEC225', 'Enterprise Entrepreneurship I', 'DEPT002', 'LEVEL2', 2, 'PRAC_150', 3, 1, 0, 6, 150, 30, 45, 30, 45),
('MEC-Y3S1-01', 'MEC311', 'Mathematics for Technologist', 'DEPT002', 'LEVEL3', 1, 'THEO_150', 3, 1, 4, 0, 150, 30, 45, 30, 45),
('MEC-Y3S1-02', 'MEC312', '3D CAD and Mechanical Modeling', 'DEPT002', 'LEVEL3', 1, 'PRAC_150', 3, 1, 0, 6, 150, 30, 45, 30, 45),
('MEC-Y3S1-03', 'MEC313', 'Automatic Control', 'DEPT002', 'LEVEL3', 1, 'PRAC_150', 3, 2, 0, 3, 150, 30, 45, 30, 45),
('MEC-Y3S1-04', 'MEC314', 'Robotic Systems', 'DEPT002', 'LEVEL3', 1, 'PRAC_150', 4, 2, 2, 3, 150, 30, 45, 30, 45),
('MEC-Y3S1-05', 'MEC315', 'Sensors and Instrumentation', 'DEPT002', 'LEVEL3', 1, 'PRAC_150', 3, 2, 0, 4, 150, 30, 45, 30, 45),
('MEC-Y3S2-01', 'MEC321', 'Power Electronics and Drives', 'DEPT002', 'LEVEL3', 2, 'PRAC_150', 3, 2, 0, 4, 150, 30, 45, 30, 45),
('MEC-Y3S2-02', 'MEC322', 'CNC Technology', 'DEPT002', 'LEVEL3', 2, 'PRAC_150', 3, 2, 0, 4, 150, 30, 45, 30, 45),
('MEC-Y3S2-03', 'MEC323', 'Mechanism Design', 'DEPT002', 'LEVEL3', 2, 'PRAC_150', 3, 2, 0, 4, 150, 30, 45, 30, 45),
('MEC-Y3S2-04', 'MEC324', 'Microprocessor Technology', 'DEPT002', 'LEVEL3', 2, 'PRAC_150', 4, 2, 2, 4, 150, 30, 45, 30, 45),
('MEC-Y3S2-05', 'MEC325', 'Production System Planning Control', 'DEPT002', 'LEVEL3', 2, 'PRAC_150', 3, 2, 0, 3, 150, 30, 45, 30, 45),
('MEC-Y4S1-01', 'MEC411', 'Business Communication and Ethics', 'DEPT002', 'LEVEL4', 1, 'THEO_50', 1, 1, 0, 0, 50, 10, 10, 10, 20),
('MEC-Y4S1-02', 'MEC412', 'Embedded Systems', 'DEPT002', 'LEVEL4', 1, 'PRAC_150', 3, 2, 0, 4, 150, 30, 45, 30, 45),
('MEC-Y4S1-03', 'MEC413', 'Digital Signal Processing', 'DEPT002', 'LEVEL4', 1, 'PRAC_150', 3, 2, 0, 4, 150, 30, 45, 30, 45),
('MEC-Y4S1-04', 'MEC414', 'Dynamic Systems Modeling', 'DEPT002', 'LEVEL4', 1, 'PRAC_150', 3, 2, 0, 4, 150, 30, 45, 30, 45),
('MEC-Y4S1-05', 'MEC415', 'Metrology and Quality Control', 'DEPT002', 'LEVEL4', 1, 'PRAC_150', 3, 2, 0, 4, 150, 30, 45, 30, 45),
('MEC-Y4S1-06', 'MEC416', 'Human-Machine Interface Technology', 'DEPT002', 'LEVEL4', 1, 'PRAC_150', 3, 2, 0, 4, 150, 30, 45, 30, 45),
('MEC-Y4S2-01', 'MEC421', 'Advanced PLC', 'DEPT002', 'LEVEL4', 2, 'PRAC_150', 4, 2, 2, 4, 150, 30, 45, 30, 45),
('MEC-Y4S2-02', 'MEC422', 'Enterprise Entrepreneurship II', 'DEPT002', 'LEVEL4', 2, 'PRAC_150', 4, 2, 0, 6, 150, 30, 45, 30, 45),
('MEC-Y4S2-03', 'MEC423', 'Graduation Project', 'DEPT002', 'LEVEL4', 2, 'PROJECT_150', 4, 2, 0, 6, 150, 30, 30, 45, 45);

-- ================================================================================
-- PART 5: AUTOTRONICS COURSES (48 courses)
-- ================================================================================

INSERT INTO courses (id, code, name_ar, name_en, department_id, academic_level_id, semester_number, course_type_id, credit_hours, max_marks, assignment1_marks, assignment2_marks, year_work_marks, final_exam_marks) VALUES
('AUT-Y1S1-01', 'AUT111', 'لغة إنجليزية 1', 'English Language 1', 'DEPT004', 'LEVEL1', 1, 'THEO_50', 1, 50, 10, 10, 10, 20),
('AUT-Y1S1-02', 'AUT112', 'رياضيات 1', 'Mathematics 1', 'DEPT004', 'LEVEL1', 1, 'THEO_150', 3, 150, 30, 45, 30, 45),
('AUT-Y1S1-03', 'AUT113', 'فيزياء', 'Physics', 'DEPT004', 'LEVEL1', 1, 'PRAC_150', 3, 150, 30, 30, 45, 45),
('AUT-Y1S1-04', 'AUT114', 'تكنولوجيا المعلومات', 'Information Technology', 'DEPT004', 'LEVEL1', 1, 'PRAC_150', 3, 150, 10, 30, 0, 60),
('AUT-Y1S1-05', 'AUT115', 'رسم فني', 'Technical Drawing', 'DEPT004', 'LEVEL1', 1, 'THEO_100', 2, 100, 20, 20, 20, 40),
('AUT-Y1S1-06', 'AUT116', 'أساسيات ورش', 'Workshop Basics', 'DEPT004', 'LEVEL1', 1, 'PRAC_200', 4, 200, 20, 0, 50, 80),
('AUT-Y1S2-01', 'AUT121', 'لغة إنجليزية 2', 'English Language 2', 'DEPT004', 'LEVEL1', 2, 'THEO_50', 1, 50, 10, 10, 10, 20),
('AUT-Y1S2-02', 'AUT122', 'رياضيات 2', 'Mathematics 2', 'DEPT004', 'LEVEL1', 2, 'THEO_150', 3, 150, 30, 45, 30, 45),
('AUT-Y1S2-03', 'AUT123', 'السلامة والصحة المهنية', 'Occupational Health and Safety', 'DEPT004', 'LEVEL1', 2, 'THEO_50', 1, 50, 10, 0, 40, 0),
('AUT-Y1S2-04', 'AUT124', 'أساسيات الهيدروليكا', 'Hydraulics Basics', 'DEPT004', 'LEVEL1', 2, 'PRAC_150', 3, 150, 20, 40, 0, 90),
('AUT-Y1S2-05', 'AUT125', 'محركات السيارات 1', 'Car Engines 1', 'DEPT004', 'LEVEL1', 2, 'PRAC_150', 3, 150, 20, 0, 50, 80),
('AUT-Y1S2-06', 'AUT126', 'مبادئ كهرباء وإلكترونيات السيارات', 'Automotive Electrical and Electronics Principles', 'DEPT004', 'LEVEL1', 2, 'PRAC_150', 3, 150, 20, 20, 0, 60),
('AUT-Y1S2-07', 'AUT127', 'تدريب عملي', 'Practical Training', 'DEPT004', 'LEVEL1', 2, 'PRAC_100', 2, 100, 0, 0, 100, 0),
('AUT-Y2S1-01', 'AUT211', 'لغة إنجليزية 3', 'English Language 3', 'DEPT004', 'LEVEL2', 1, 'THEO_50', 1, 50, 10, 10, 10, 20),
('AUT-Y2S1-02', 'AUT212', 'ديناميكا حرارية', 'Thermodynamics', 'DEPT004', 'LEVEL2', 1, 'PRAC_150', 3, 150, 20, 40, 0, 90),
('AUT-Y2S1-03', 'AUT213', 'المواد الهندسية واختباراتها', 'Engineering Materials and Testing', 'DEPT004', 'LEVEL2', 1, 'PRAC_100', 2, 100, 20, 20, 0, 60),
('AUT-Y2S1-04', 'AUT214', 'محركات السيارات 2', 'Car Engines 2', 'DEPT004', 'LEVEL2', 1, 'PRAC_150', 3, 150, 20, 0, 40, 90),
('AUT-Y2S1-05', 'AUT215', 'منظومات نقل الحركة التقليدية', 'Traditional Transmission Systems', 'DEPT004', 'LEVEL2', 1, 'PRAC_200', 4, 200, 20, 0, 60, 120),
('AUT-Y2S1-06', 'AUT216', 'أنظمة كهرباء محرك السيارة', 'Car Engine Electrical Systems', 'DEPT004', 'LEVEL2', 1, 'PRAC_150', 3, 150, 20, 0, 40, 90),
('AUT-Y2S2-01', 'AUT221', 'كتابة التقارير الفنية', 'Technical Report Writing', 'DEPT004', 'LEVEL2', 2, 'THEO_50', 1, 50, 10, 10, 10, 20),
('AUT-Y2S2-02', 'AUT222', 'علاقات عمل ومهارات الاتصال', 'Work Relations and Communication Skills', 'DEPT004', 'LEVEL2', 2, 'THEO_100', 2, 100, 20, 0, 0, 80),
('AUT-Y2S2-03', 'AUT223', 'الإرشاد والتوجيه وريادة الأعمال', 'Guidance and Entrepreneurship', 'DEPT004', 'LEVEL2', 2, 'THEO_100', 2, 100, 20, 0, 0, 80),
('AUT-Y2S2-04', 'AUT224', 'أنظمة الفرامل', 'Brake Systems', 'DEPT004', 'LEVEL2', 2, 'PRAC_200', 4, 200, 20, 0, 60, 120),
('AUT-Y2S2-05', 'AUT225', 'أنظمة التعليق والتوجيه', 'Suspension and Steering Systems', 'DEPT004', 'LEVEL2', 2, 'PRAC_200', 4, 200, 20, 0, 60, 120),
('AUT-Y2S2-06', 'AUT226', 'صيانة وإصلاح المحركات', 'Engine Maintenance and Repair', 'DEPT004', 'LEVEL2', 2, 'PRAC_150', 3, 150, 20, 0, 40, 90),
('AUT-Y3S1-01', 'AUT311', 'لغة إنجليزية', 'English Language', 'DEPT004', 'LEVEL3', 1, 'THEO_50', 1, 50, 10, 10, 10, 20),
('AUT-Y3S1-02', 'AUT312', 'ميكانيكا الموائع', 'Fluid Mechanics', 'DEPT004', 'LEVEL3', 1, 'PRAC_150', 3, 150, 20, 0, 40, 90),
('AUT-Y3S1-03', 'AUT313', 'دوائر الإشعال الإلكترونية بالسيارات', 'Electronic Ignition Circuits in Cars', 'DEPT004', 'LEVEL3', 1, 'PRAC_200', 4, 200, 20, 0, 60, 120),
('AUT-Y3S1-04', 'AUT314', 'منظومات وقود البنزين', 'Gasoline Fuel Systems', 'DEPT004', 'LEVEL3', 1, 'PRAC_150', 3, 150, 20, 0, 40, 90),
('AUT-Y3S1-05', 'AUT315', 'منظومات وقود الديزل', 'Diesel Fuel Systems', 'DEPT004', 'LEVEL3', 1, 'PRAC_100', 2, 100, 10, 0, 30, 60),
('AUT-Y3S1-06', 'AUT316', 'ديناميكا المركبات 1', 'Vehicle Dynamics 1', 'DEPT004', 'LEVEL3', 1, 'PRAC_150', 3, 150, 30, 30, 45, 45),
('AUT-Y3S2-01', 'AUT321', 'لغة إنجليزية', 'English Language', 'DEPT004', 'LEVEL3', 2, 'THEO_50', 1, 50, 10, 10, 10, 20),
('AUT-Y3S2-02', 'AUT322', 'دوائر كهربائية ورفاهية بالمركبات', 'Electrical and Comfort Circuits in Vehicles', 'DEPT004', 'LEVEL3', 2, 'PRAC_150', 3, 150, 20, 0, 40, 90),
('AUT-Y3S2-03', 'AUT323', 'منظومات الأمن والسلامة بالمركبات', 'Security and Safety Systems in Vehicles', 'DEPT004', 'LEVEL3', 2, 'PRAC_150', 3, 150, 20, 0, 40, 90),
('AUT-Y3S2-04', 'AUT324', 'ناقل الحركة الأتوماتيكي', 'Automatic Transmission', 'DEPT004', 'LEVEL3', 2, 'PRAC_150', 3, 150, 20, 0, 60, 120),
('AUT-Y3S2-05', 'AUT325', 'منظومات التكييف والتدفئة', 'Air Conditioning and Heating Systems', 'DEPT004', 'LEVEL3', 2, 'PRAC_150', 3, 150, 20, 0, 0, 80),
('AUT-Y3S2-06', 'AUT326', 'ديناميكا المركبات 2', 'Vehicle Dynamics 2', 'DEPT004', 'LEVEL3', 2, 'PRAC_150', 3, 150, 20, 0, 0, 90),
('AUT-Y4S1-01', 'AUT411', 'لغة إنجليزية', 'English Language', 'DEPT004', 'LEVEL4', 1, 'PRAC_100', 2, 100, 20, 20, 0, 60),
('AUT-Y4S1-02', 'AUT412', 'منظومات التحكم الإلكتروني بالمحرك', 'Electronic Engine Control Systems', 'DEPT004', 'LEVEL4', 1, 'PRAC_250', 5, 250, 30, 0, 70, 150),
('AUT-Y4S1-03', 'AUT413', 'منظومات التحكم الإلكتروني بالتعليق والتوجيه والفرامل', 'Electronic Control Systems for Suspension, Steering and Brakes', 'DEPT004', 'LEVEL4', 1, 'PRAC_200', 4, 200, 20, 0, 60, 120),
('AUT-Y4S1-04', 'AUT414', 'إدارة نظم النقل والمرور', 'Transportation and Traffic Management', 'DEPT004', 'LEVEL4', 1, 'THEO_100', 2, 100, 20, 0, 0, 80),
('AUT-Y4S1-05', 'AUT415', 'رسم بالحاسب', 'Computer Drawing', 'DEPT004', 'LEVEL4', 1, 'PRAC_150', 3, 150, 20, 40, 0, 90),
('AUT-Y4S2-01', 'AUT421', 'لغة إنجليزية', 'English Language', 'DEPT004', 'LEVEL4', 2, 'THEO_50', 1, 50, 10, 10, 10, 20),
('AUT-Y4S2-02', 'AUT422', 'نظم إلكترونية مدمجة', 'Embedded Electronic Systems', 'DEPT004', 'LEVEL4', 2, 'PRAC_150', 3, 150, 20, 40, 0, 90),
('AUT-Y4S2-03', 'AUT423', 'محركات الطاقة البديلة والمركبات الهجينة', 'Alternative Energy Engines and Hybrid Vehicles', 'DEPT004', 'LEVEL4', 2, 'PRAC_150', 3, 150, 20, 0, 60, 120),
('AUT-Y4S2-04', 'AUT424', 'تخطيط وإدارة مراكز الصيانة', 'Maintenance Center Planning and Management', 'DEPT004', 'LEVEL4', 2, 'THEO_100', 2, 100, 20, 0, 0, 80),
('AUT-Y4S2-05', 'AUT425', 'تدريب ميداني متقدم', 'Advanced Field Training', 'DEPT004', 'LEVEL4', 2, 'PRAC_150', 3, 150, 30, 0, 120, 0),
('AUT-Y4S2-06', 'AUT426', 'مشروع تخرج', 'Graduation Project', 'DEPT004', 'LEVEL4', 2, 'PRAC_200', 4, 200, 40, 40, 40, 80);

-- ================================================================================
-- PART 6: NEW AND RENEWABLE ENERGY COURSES (49 courses)
-- ================================================================================

INSERT INTO courses (id, code, name_en, department_id, academic_level_id, semester_number, course_type_id, credit_hours, max_marks, year_work_marks, assignment1_marks, assignment2_marks, final_exam_marks) VALUES
('REN-Y1S1-01', 'UNI111', 'English Language I', 'DEPT001', 'LEVEL1', 1, 'THEO_50', 1, 50, 10, 10, 10, 20),
('REN-Y1S1-02', 'UNI112', 'Mathematics', 'DEPT001', 'LEVEL1', 1, 'THEO_150', 3, 150, 30, 45, 30, 45),
('REN-Y1S1-03', 'REN113', 'Renewable Energy', 'DEPT001', 'LEVEL1', 1, 'PRAC_150', 2, 150, 30, 30, 45, 45),
('REN-Y1S1-04', 'FAC114', 'CAD for Engineering', 'DEPT001', 'LEVEL1', 1, 'PRAC_150', 3, 150, 30, 30, 45, 45),
('REN-Y1S1-05', 'ELE115', 'Electrical and Electronic Principles', 'DEPT001', 'LEVEL1', 1, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('REN-Y1S1-06', 'UNI116', 'Programming using MATLAB', 'DEPT001', 'LEVEL1', 1, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('REN-Y1S2-01', 'UNI1Y1', 'English Language II', 'DEPT001', 'LEVEL1', 2, 'THEO_50', 1, 50, 10, 10, 10, 20),
('REN-Y1S2-02', 'UNI1Y2', 'Further Mathematics', 'DEPT001', 'LEVEL1', 2, 'THEO_150', 3, 150, 30, 45, 30, 45),
('REN-Y1S2-03', 'MEC123', 'Mechanical Principles', 'DEPT001', 'LEVEL1', 2, 'THEO_150', 3, 150, 30, 45, 30, 45),
('REN-Y1S2-04', 'MEC124', 'Workshop Practices', 'DEPT001', 'LEVEL1', 2, 'PRAC_150', 3, 150, 30, 30, 45, 45),
('REN-Y1S2-05', 'ELE125', 'Electronic Principles', 'DEPT001', 'LEVEL1', 2, 'PRAC_150', 2, 150, 30, 45, 30, 45),
('REN-Y1S2-06', 'UNI126', 'Human Rights', 'DEPT001', 'LEVEL1', 2, 'THEO_50', 2, 50, 10, 10, 10, 20),
('REN-Y1S2-07', 'UNI127', 'Health and Safety', 'DEPT001', 'LEVEL1', 2, 'PRAC_100', 2, 100, 20, 20, 20, 40),
('REN-Y2S1-01', 'UNI211', 'English Language III', 'DEPT001', 'LEVEL2', 1, 'THEO_50', 1, 50, 10, 10, 10, 20),
('REN-Y2S1-02', 'MEC212', 'Fluid Mechanics and Turbomachines', 'DEPT001', 'LEVEL2', 1, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('REN-Y2S1-03', 'REN213', 'Renewable Energy Technology', 'DEPT001', 'LEVEL2', 1, 'PRAC_150', 2, 150, 30, 45, 30, 45),
('REN-Y2S1-04', 'MEC214', 'Applied Hydraulics and Pneumatics', 'DEPT001', 'LEVEL2', 1, 'PRAC_150', 2, 150, 30, 45, 30, 45),
('REN-Y2S1-05', 'ELE215', 'Applied DC and AC Machines', 'DEPT001', 'LEVEL2', 1, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('REN-Y2S1-06', 'FAC216', 'Graduation Project I', 'DEPT001', 'LEVEL2', 1, 'PROJECT_150', 0, 150, 30, 30, 45, 45),
('REN-Y2S2-01', 'UNI222', 'English Language IV', 'DEPT001', 'LEVEL2', 2, 'THEO_50', 1, 50, 10, 10, 10, 20),
('REN-Y2S2-02', 'MEC2Y2', 'Thermodynamics and Heat Transfer', 'DEPT001', 'LEVEL2', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('REN-Y2S2-03', 'ELE223', 'Electrical Inspection Processes and Procedures', 'DEPT001', 'LEVEL2', 2, 'PRAC_150', 2, 150, 30, 45, 30, 45),
('REN-Y2S2-04', 'REN224', 'Installation, Maintenance and Repair of Small RE Systems', 'DEPT001', 'LEVEL2', 2, 'PRAC_150', 2, 150, 30, 45, 30, 45),
('REN-Y2S2-05', 'ELE225', 'Instrumentation and Measurements', 'DEPT001', 'LEVEL2', 2, 'PRAC_150', 2, 150, 30, 45, 30, 45),
('REN-Y2S2-06', 'FAC226', 'Graduation Project II', 'DEPT001', 'LEVEL2', 2, 'PROJECT_150', 0, 150, 30, 30, 45, 45),
('REN-Y3S1-01', 'ENG311', 'English V', 'DEPT001', 'LEVEL3', 1, 'THEO_50', 1, 50, 10, 10, 10, 20),
('REN-Y3S1-02', 'SOL312', 'Solar Energy and Applications', 'DEPT001', 'LEVEL3', 1, 'PRAC_150', 2, 150, 30, 45, 30, 45),
('REN-Y3S1-03', 'SOL313', 'Solar Pumping System', 'DEPT001', 'LEVEL3', 1, 'PRAC_150', 2, 150, 30, 45, 30, 45),
('REN-Y3S1-04', 'SOL314', 'PV Technology and Thermal Characterization', 'DEPT001', 'LEVEL3', 1, 'PRAC_150', 2, 150, 30, 30, 0, 60),
('REN-Y3S1-05', 'REN315', 'Modelling and Simulation', 'DEPT001', 'LEVEL3', 1, 'PRAC_150', 2, 150, 30, 30, 45, 45),
('REN-Y3S1-06', 'REN316', 'Employability Skills', 'DEPT001', 'LEVEL3', 1, 'PRAC_150', 2, 150, 30, 30, 45, 45),
('REN-Y3S2-01', 'ENG321', 'English VI', 'DEPT001', 'LEVEL3', 2, 'THEO_50', 1, 50, 10, 10, 10, 20),
('REN-Y3S2-02', 'REN322', 'Industrial Automation', 'DEPT001', 'LEVEL3', 2, 'PRAC_150', 3, 150, 30, 30, 45, 45),
('REN-Y3S2-03', 'REN323', 'Transmission and Distribution', 'DEPT001', 'LEVEL3', 2, 'PRAC_150', 2, 150, 30, 45, 30, 45),
('REN-Y3S2-04', 'REN324', 'Power Electronics and Drives', 'DEPT001', 'LEVEL3', 2, 'PRAC_150', 2, 150, 30, 45, 30, 45),
('REN-Y3S2-05', 'SOL325', 'Solar Thermal Systems and Thermal Storage', 'DEPT001', 'LEVEL3', 2, 'PRAC_150', 3, 150, 30, 30, 45, 45),
('REN-Y3S2-06', 'SOL326', 'Installation, Testing and Commissioning of PV System', 'DEPT001', 'LEVEL3', 2, 'PRAC_150', 2, 150, 30, 45, 30, 45),
('REN-Y4S1-01', 'ENG411', 'English VII', 'DEPT001', 'LEVEL4', 1, 'THEO_50', 1, 50, 10, 10, 10, 20),
('REN-Y4S1-02', 'REN412', 'Electric Power Systems', 'DEPT001', 'LEVEL4', 1, 'PRAC_100', 2, 100, 20, 20, 20, 40),
('REN-Y4S1-03', 'REN413', 'MLN Unit', 'DEPT001', 'LEVEL4', 1, 'PRAC_150', 2, 150, 30, 45, 30, 45),
('REN-Y4S1-04', 'SOL414', 'Maintenance, Troubleshooting and Repair of PV Plants', 'DEPT001', 'LEVEL4', 1, 'PRAC_150', 2, 150, 30, 30, 45, 45),
('REN-Y4S1-05', 'REN415', 'Managing a Professional Project', 'DEPT001', 'LEVEL4', 1, 'PRAC_150', 2, 150, 30, 30, 45, 45),
('REN-Y4S1-06', 'FAC416', 'Graduation Project I', 'DEPT001', 'LEVEL4', 1, 'PROJECT_150', 0, 150, 30, 30, 45, 45),
('REN-Y4S2-01', 'ENG421', 'English VIII', 'DEPT001', 'LEVEL4', 2, 'THEO_50', 1, 50, 10, 10, 10, 20),
('REN-Y4S2-02', 'REN422', 'Smart Grid Technology', 'DEPT001', 'LEVEL4', 2, 'PRAC_150', 2, 150, 30, 30, 45, 45),
('REN-Y4S2-03', 'REN423', 'Power System Protection', 'DEPT001', 'LEVEL4', 2, 'PRAC_100', 2, 100, 20, 20, 20, 40),
('REN-Y4S2-04', 'REN424', 'MLN Unit', 'DEPT001', 'LEVEL4', 2, 'PRAC_150', 2, 150, 30, 45, 30, 45),
('REN-Y4S2-05', 'REN425', 'Energy and Building Management Systems', 'DEPT001', 'LEVEL4', 2, 'PRAC_150', 2, 150, 30, 30, 45, 45),
('REN-Y4S2-06', 'FAC426', 'Graduation Project II', 'DEPT001', 'LEVEL4', 2, 'PROJECT_150', 0, 150, 30, 30, 45, 45);

-- ================================================================================
-- PART 7: PETROLEUM TECHNOLOGY COURSES (50 courses)
-- ================================================================================

INSERT INTO courses (id, code, name_en, department_id, academic_level_id, semester_number, course_type_id, credit_hours, max_marks, year_work_marks, assignment1_marks, assignment2_marks, final_exam_marks) VALUES
('PET-Y1S1-01', 'PET111', 'Human Rights', 'DEPT005', 'LEVEL1', 1, 'THEO_50', 1, 50, 10, 10, 10, 20),
('PET-Y1S1-02', 'PET112', 'Basic Health, Safety and Environment', 'DEPT005', 'LEVEL1', 1, 'PRAC_100', 2, 100, 20, 20, 20, 40),
('PET-Y1S1-03', 'PET113', 'Mathematics', 'DEPT005', 'LEVEL1', 1, 'THEO_150', 3, 150, 30, 45, 30, 45),
('PET-Y1S1-04', 'PET114', 'Computer Skills', 'DEPT005', 'LEVEL1', 1, 'PRAC_100', 2, 100, 20, 20, 20, 40),
('PET-Y1S1-05', 'PET115', 'English 1', 'DEPT005', 'LEVEL1', 1, 'THEO_50', 1, 50, 10, 10, 10, 20),
('PET-Y1S1-06', 'PET116', 'Basic Mechanics and Engineering Drawings', 'DEPT005', 'LEVEL1', 1, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y1S1-07', 'PET117', 'Introduction to Petroleum Industry', 'DEPT005', 'LEVEL1', 1, 'THEO_100', 2, 100, 20, 20, 20, 40),
('PET-Y1S2-01', 'PET121', 'Basic Health, Safety and Environment', 'DEPT005', 'LEVEL1', 2, 'PRAC_100', 2, 100, 20, 20, 20, 40),
('PET-Y1S2-02', 'PET122', 'Electrical and Electronic Principles', 'DEPT005', 'LEVEL1', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y1S2-03', 'PET123', 'Principles of Instrumentation and Automation', 'DEPT005', 'LEVEL1', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y1S2-04', 'PET124', 'Thermofluids', 'DEPT005', 'LEVEL1', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y1S2-05', 'PET125', 'English 2', 'DEPT005', 'LEVEL1', 2, 'THEO_50', 1, 50, 10, 10, 10, 20),
('PET-Y1S2-06', 'PET126', 'Hydrocarbon Chemistry and Behavior', 'DEPT005', 'LEVEL1', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y2S1-01', 'PET211', 'Health, Safety and Environment - Surface Production', 'DEPT005', 'LEVEL2', 1, 'PRAC_100', 2, 100, 20, 20, 20, 40),
('PET-Y2S1-02', 'PET212', 'Quality Control, Corrosion and Hydrocarbon Analyses', 'DEPT005', 'LEVEL2', 1, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y2S1-03', 'PET213', 'Technology of Oil and Gas Plants Operation and Control', 'DEPT005', 'LEVEL2', 1, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y2S1-04', 'PET214', 'Technology of Natural Gas Processing and Compression', 'DEPT005', 'LEVEL2', 1, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y2S1-05', 'PET215', 'English 3', 'DEPT005', 'LEVEL2', 1, 'THEO_50', 1, 50, 10, 10, 10, 20),
('PET-Y2S1-06', 'PET216', 'Technology of Offshore and Subsea Operation', 'DEPT005', 'LEVEL2', 1, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y2S2-01', 'PET221', 'Health, Safety and Environment - Surface Production', 'DEPT005', 'LEVEL2', 2, 'PRAC_100', 2, 100, 20, 20, 20, 40),
('PET-Y2S2-02', 'PET222', 'Water Injection Principles, Operation and Treatment', 'DEPT005', 'LEVEL2', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y2S2-03', 'PET223', 'Oil Storage Tanks, Pipelines and Valves', 'DEPT005', 'LEVEL2', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y2S2-04', 'PET224', 'Oil and Gas Wells Production Operation and Well Testing', 'DEPT005', 'LEVEL2', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y2S2-05', 'PET225', 'Artificial Lift Systems and Surface Pumps', 'DEPT005', 'LEVEL2', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y2S2-06', 'PET226', 'Basics of Preventive and Predictive Maintenance', 'DEPT005', 'LEVEL2', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y3S1-01', 'PET311', 'Safety in Oil Process', 'DEPT005', 'LEVEL3', 1, 'PRAC_100', 2, 100, 20, 20, 20, 40),
('PET-Y3S1-02', 'PET312', 'Reading Catalogues and Instructions', 'DEPT005', 'LEVEL3', 1, 'THEO_100', 2, 100, 20, 20, 20, 40),
('PET-Y3S1-03', 'PET313', 'Basics of Material Selection for Petroleum Industry', 'DEPT005', 'LEVEL3', 1, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y3S1-04', 'PET314', 'Basic Natural Gas Analysis and Measurements', 'DEPT005', 'LEVEL3', 1, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y3S1-05', 'PET315', 'Basics of Well Drilling, Completion and Workover', 'DEPT005', 'LEVEL3', 1, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y3S1-06', 'PET316', 'Onshore and Offshore Wellhead', 'DEPT005', 'LEVEL3', 1, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y3S1-07', 'PET317', 'Sucker Rod and Jet Pumping System Installation, Operation and Maintenance', 'DEPT005', 'LEVEL3', 1, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y3S2-01', 'PET321', 'Process Safety and Environmental Protection 2', 'DEPT005', 'LEVEL3', 2, 'PRAC_100', 2, 100, 20, 20, 20, 40),
('PET-Y3S2-02', 'PET322', 'Gas Conditioning Processing', 'DEPT005', 'LEVEL3', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y3S2-03', 'PET323', 'Petroleum Production Measurements and Accounting', 'DEPT005', 'LEVEL3', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y3S2-04', 'PET324', 'Technical Report Writing', 'DEPT005', 'LEVEL3', 2, 'THEO_100', 2, 100, 20, 20, 20, 40),
('PET-Y3S2-05', 'PET325', 'Codes and Standards Used in Oil and Gas Industry', 'DEPT005', 'LEVEL3', 2, 'THEO_100', 2, 100, 20, 20, 20, 40),
('PET-Y3S2-06', 'PET326', 'Gas Chromatographic Analysis of Crude Oil', 'DEPT005', 'LEVEL3', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y3S2-07', 'PET327', 'Basics of Preventive and Predictive Maintenance', 'DEPT005', 'LEVEL3', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y4S1-01', 'PET411', 'EGPC Safety Pillars in Oil Process', 'DEPT005', 'LEVEL4', 1, 'PRAC_100', 2, 100, 20, 20, 20, 40),
('PET-Y4S1-02', 'PET412', 'Oil and Gas Process Plants Troubleshooting and Problem Solving', 'DEPT005', 'LEVEL4', 1, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y4S1-03', 'PET413', 'Materials Procurements and Services Tendering Process', 'DEPT005', 'LEVEL4', 1, 'THEO_100', 2, 100, 20, 20, 20, 40),
('PET-Y4S1-04', 'PET414', 'Valves Maintenance, Operation, Troubleshooting and Repair', 'DEPT005', 'LEVEL4', 1, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y4S1-05', 'PET415', 'Advanced Instrumentation Fundamentals and Techniques Used in Crude Oil Analysis', 'DEPT005', 'LEVEL4', 1, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y4S2-01', 'PET421', 'Process Safety and Environmental Protection 4', 'DEPT005', 'LEVEL4', 2, 'PRAC_100', 2, 100, 20, 20, 20, 40),
('PET-Y4S2-02', 'PET422', 'Oil and Gas Pipelines Operation, Monitoring and Maintenance', 'DEPT005', 'LEVEL4', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y4S2-03', 'PET423', 'Oral Presentation Skills', 'DEPT005', 'LEVEL4', 2, 'THEO_100', 2, 100, 20, 20, 20, 40),
('PET-Y4S2-04', 'PET424', 'Sampling and Analysis of Water', 'DEPT005', 'LEVEL4', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y4S2-05', 'PET425', 'Automation and Control Systems for Upstream Petroleum Production', 'DEPT005', 'LEVEL4', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45),
('PET-Y4S2-06', 'PET426', 'ESP and PCP Systems Installation, Operation and Maintenance', 'DEPT005', 'LEVEL4', 2, 'PRAC_150', 3, 150, 30, 45, 30, 45);

-- ================================================================================
-- PART 8: FEES MANAGEMENT SYSTEM
-- ================================================================================

-- Student Fees table
CREATE TABLE student_fees (
    id INT AUTO_INCREMENT PRIMARY KEY,
    student_id VARCHAR(20) NOT NULL,
    academic_year VARCHAR(9) NOT NULL,
    semester_id VARCHAR(20) NOT NULL,
    base_fee DECIMAL(10,2) NOT NULL DEFAULT 15000.00,
    additional_fees DECIMAL(10,2) DEFAULT 0.00,
    discount DECIMAL(10,2) DEFAULT 0.00,
    total_fees DECIMAL(10,2) DEFAULT 0.00,
    paid_amount DECIMAL(10,2) DEFAULT 0.00,
    remaining_amount DECIMAL(10,2) DEFAULT 0.00,
    status ENUM('unpaid', 'partial', 'paid') DEFAULT 'unpaid',
    due_date DATE,
    notes TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (student_id) REFERENCES students(id) ON DELETE CASCADE,
    FOREIGN KEY (semester_id) REFERENCES semesters(id),
    UNIQUE KEY unique_student_semester (student_id, semester_id),
    INDEX idx_status (status),
    INDEX idx_academic_year (academic_year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Fee Payments table
CREATE TABLE fee_payments (
    id INT AUTO_INCREMENT PRIMARY KEY,
    student_fee_id INT NOT NULL,
    payment_date DATE NOT NULL,
    amount DECIMAL(10,2) NOT NULL,
    payment_method ENUM('cash', 'bank_transfer', 'card', 'other') DEFAULT 'cash',
    receipt_number VARCHAR(50),
    recorded_by INT NOT NULL,
    notes TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (student_fee_id) REFERENCES student_fees(id) ON DELETE CASCADE,
    FOREIGN KEY (recorded_by) REFERENCES users(id),
    INDEX idx_payment_date (payment_date)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Add fee permissions to the permissions table
INSERT INTO permissions (id, name, description) VALUES
('PERM_FEE_VIEW', 'fee.view', 'View student fees'),
('PERM_FEE_CREATE', 'fee.create', 'Create fee records'),
('PERM_FEE_EDIT', 'fee.edit', 'Edit fee records'),
('PERM_FEE_DELETE', 'fee.delete', 'Delete fee records'),
('PERM_PAY_CREATE', 'payment.create', 'Record payments'),
('PERM_PAY_VIEW', 'payment.view', 'View payment history');

-- Grant fee permissions to Student Affairs
INSERT INTO role_permissions (role_id, permission_id) VALUES
('ROLE_STUDENT_AFFAIRS', 'PERM_FEE_VIEW'),
('ROLE_STUDENT_AFFAIRS', 'PERM_FEE_CREATE'),
('ROLE_STUDENT_AFFAIRS', 'PERM_FEE_EDIT'),
('ROLE_STUDENT_AFFAIRS', 'PERM_PAY_CREATE'),
('ROLE_STUDENT_AFFAIRS', 'PERM_PAY_VIEW');

-- Trigger to update fee status automatically after payment
DELIMITER //

CREATE TRIGGER before_insert_student_fees
BEFORE INSERT ON student_fees
FOR EACH ROW
BEGIN
    SET NEW.total_fees = NEW.base_fee + NEW.additional_fees - NEW.discount;
    SET NEW.remaining_amount = NEW.total_fees - NEW.paid_amount;
END//

CREATE TRIGGER before_update_student_fees
BEFORE UPDATE ON student_fees
FOR EACH ROW
BEGIN
    SET NEW.total_fees = NEW.base_fee + NEW.additional_fees - NEW.discount;
    SET NEW.remaining_amount = NEW.total_fees - NEW.paid_amount;
END//

CREATE TRIGGER update_fee_status_after_payment 
AFTER INSERT ON fee_payments
FOR EACH ROW
BEGIN
    DECLARE new_paid DECIMAL(10,2);
    DECLARE total DECIMAL(10,2);
    
    SELECT paid_amount + NEW.amount, total_fees 
    INTO new_paid, total
    FROM student_fees 
    WHERE id = NEW.student_fee_id;
    
    UPDATE student_fees 
    SET paid_amount = new_paid,
        status = CASE 
            WHEN new_paid >= total THEN 'paid'
            WHEN new_paid > 0 THEN 'partial'
            ELSE 'unpaid'
        END
    WHERE id = NEW.student_fee_id;
END//
DELIMITER ;

SELECT 'Schema v3 created successfully!' AS status;
