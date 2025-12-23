-- ============================================
-- Student Information System (SIS) Database
-- NCTU - New Cairo Technological University
-- MySQL 8.0
-- ============================================
--
-- NOTE:
-- This is a legacy (v1) schema kept for reference.
-- The application code (v2 + RBAC) expects `sql/schema_v2.sql`.
-- ============================================

-- Drop and recreate database to start fresh
DROP DATABASE IF EXISTS sis_database;

CREATE DATABASE sis_database
CHARACTER SET utf8mb4
COLLATE utf8mb4_unicode_ci;

USE sis_database;

-- ============================================
-- TABLE: academic_levels
-- ============================================
CREATE TABLE academic_levels (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
    year INT NOT NULL CHECK (year BETWEEN 1 AND 6),
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: colleges
-- ============================================
CREATE TABLE colleges (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: departments
-- ============================================
CREATE TABLE departments (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    college_id VARCHAR(20) NOT NULL,
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (college_id) REFERENCES colleges(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: semesters
-- ============================================
CREATE TABLE semesters (
    id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    year INT NOT NULL,
    semester_number INT NOT NULL,
    is_active BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: students
-- ============================================
CREATE TABLE students (
    id VARCHAR(20) PRIMARY KEY,
    first_name VARCHAR(50) NOT NULL,
    last_name VARCHAR(50) NOT NULL,
    date_of_birth DATE,
    gender ENUM('Male', 'Female') NOT NULL,
    email VARCHAR(100) UNIQUE,
    phone VARCHAR(20),
    registration_date DATE DEFAULT (CURRENT_DATE),
    academic_level_id VARCHAR(20),
    department_id VARCHAR(20),
    password_hash VARCHAR(255) DEFAULT 'student123',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (academic_level_id) REFERENCES academic_levels(id) ON DELETE SET NULL,
    FOREIGN KEY (department_id) REFERENCES departments(id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: professors
-- ============================================
CREATE TABLE professors (
    id VARCHAR(20) PRIMARY KEY,
    first_name VARCHAR(50) NOT NULL,
    last_name VARCHAR(50) NOT NULL,
    email VARCHAR(100) UNIQUE,
    phone VARCHAR(20),
    department_id VARCHAR(20),
    hire_date DATE DEFAULT (CURRENT_DATE),
    specialization VARCHAR(100),
    password_hash VARCHAR(255) DEFAULT 'prof123',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (department_id) REFERENCES departments(id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: courses
-- ============================================
CREATE TABLE courses (
    id VARCHAR(20) PRIMARY KEY,
    code VARCHAR(20) NOT NULL UNIQUE,
    name VARCHAR(100) NOT NULL,
    description TEXT,
    department_id VARCHAR(20),
    academic_level_id VARCHAR(20),
    course_type ENUM('theoretical', 'practical') NOT NULL DEFAULT 'theoretical',
    max_marks INT NOT NULL DEFAULT 100,
    credit_hours INT NOT NULL DEFAULT 3,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (department_id) REFERENCES departments(id) ON DELETE SET NULL,
    FOREIGN KEY (academic_level_id) REFERENCES academic_levels(id) ON DELETE SET NULL
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
    lab_type ENUM('Computer', 'Physics', 'Chemistry', 'Electronics', 'Mechatronics', 'Other') DEFAULT 'Computer',
    equipment_count INT DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: schedules
-- ============================================
CREATE TABLE schedules (
    id VARCHAR(20) PRIMARY KEY,
    day_of_week ENUM('Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday') NOT NULL,
    start_time TIME NOT NULL,
    end_time TIME NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CHECK (end_time > start_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: sections
-- ============================================
CREATE TABLE sections (
    id VARCHAR(20) PRIMARY KEY,
    course_id VARCHAR(20) NOT NULL,
    section_type ENUM('lecture', 'lab', 'tutorial') NOT NULL,
    hall_id VARCHAR(20),
    lab_id VARCHAR(20),
    professor_id VARCHAR(20),
    schedule_id VARCHAR(20),
    max_students INT DEFAULT 50,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE,
    FOREIGN KEY (hall_id) REFERENCES lecture_halls(id) ON DELETE SET NULL,
    FOREIGN KEY (lab_id) REFERENCES laboratories(id) ON DELETE SET NULL,
    FOREIGN KEY (professor_id) REFERENCES professors(id) ON DELETE SET NULL,
    FOREIGN KEY (schedule_id) REFERENCES schedules(id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: registrations
-- ============================================
CREATE TABLE registrations (
    id VARCHAR(20) PRIMARY KEY,
    student_id VARCHAR(20) NOT NULL,
    course_id VARCHAR(20) NOT NULL,
    section_id VARCHAR(20),
    registration_date DATE DEFAULT (CURRENT_DATE),
    academic_year VARCHAR(20) NOT NULL,
    semester ENUM('Fall', 'Spring', 'Summer') NOT NULL,
    status ENUM('Active', 'Dropped', 'Completed') DEFAULT 'Active',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (student_id) REFERENCES students(id) ON DELETE CASCADE,
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE,
    FOREIGN KEY (section_id) REFERENCES sections(id) ON DELETE SET NULL,
    UNIQUE KEY unique_registration (student_id, course_id, academic_year, semester)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: grades
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
    evaluation ENUM('Excellent', 'Very Good', 'Good', 'Pass', 'Fail') DEFAULT 'Fail',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (registration_id) REFERENCES registrations(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: attendance
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
    FOREIGN KEY (recorded_by) REFERENCES professors(id) ON DELETE SET NULL,
    UNIQUE KEY unique_attendance (registration_id, attendance_date)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- TABLE: admins
-- ============================================
CREATE TABLE admins (
    id VARCHAR(20) PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL DEFAULT 'admin123',
    full_name VARCHAR(100) NOT NULL,
    email VARCHAR(100),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ============================================
-- NCTU DATA
-- ============================================

-- Admin
INSERT INTO admins (id, username, password_hash, full_name, email) VALUES
('ADMIN001', 'admin', 'admin123', 'System Administrator', 'admin@nctu.edu.eg');

-- Academic Levels
INSERT INTO academic_levels (id, name, year, description) VALUES
('LEVEL1', 'First Year', 1, 'Freshman'),
('LEVEL2', 'Second Year', 2, 'Sophomore'),
('LEVEL3', 'Third Year', 3, 'Junior'),
('LEVEL4', 'Fourth Year', 4, 'Senior');

-- Colleges
INSERT INTO colleges (id, name, description) VALUES
('COL001', 'College of Industry and Energy Technology', 'NCTU Main College'),
('COL002', 'College of Prosthetics', 'Under Construction');

-- Departments
INSERT INTO departments (id, name, college_id, description) VALUES
('DEPT001', 'New and Renewable Energy', 'COL001', 'Solar, Wind, Hydro Power'),
('DEPT002', 'Mechatronics', 'COL001', 'Robotics, PLC, Automation'),
('DEPT003', 'Information Technology', 'COL001', 'Software, Networks, AI'),
('DEPT004', 'Autotronics', 'COL001', 'Automotive Electronics'),
('DEPT005', 'Petroleum Technology', 'COL001', 'Oil and Gas'),
('DEPT006', 'Prosthetics', 'COL002', 'Under Construction');

-- Semesters
INSERT INTO semesters (id, name, year, semester_number, is_active) VALUES
('2025-1', 'Fall 2025-2026', 2025, 1, TRUE),
('2025-2', 'Spring 2025-2026', 2025, 2, FALSE);

-- Lecture Halls
INSERT INTO lecture_halls (id, name, building, floor, max_capacity, has_projector, has_ac) VALUES
('HALL001', 'Hall A101', 'Building A', 1, 100, TRUE, TRUE),
('HALL002', 'Hall A102', 'Building A', 1, 80, TRUE, TRUE),
('HALL003', 'Hall B201', 'Building B', 2, 60, TRUE, TRUE),
('HALL004', 'Hall B202', 'Building B', 2, 50, TRUE, TRUE);

-- Laboratories
INSERT INTO laboratories (id, name, building, floor, max_capacity, lab_type, equipment_count) VALUES
('LAB001', 'Computer Lab 1', 'Building C', 1, 40, 'Computer', 40),
('LAB002', 'Computer Lab 2', 'Building C', 1, 35, 'Computer', 35),
('LAB003', 'Mechatronics Lab', 'Building D', 1, 30, 'Mechatronics', 25),
('LAB004', 'Electronics Lab', 'Building D', 2, 30, 'Electronics', 30),
('LAB005', 'Renewable Energy Lab', 'Building E', 1, 25, 'Other', 20);

-- ============================================
-- COURSES - Information Technology (DEPT003)
-- ============================================
INSERT INTO courses (id, code, name, department_id, academic_level_id, course_type, max_marks, credit_hours) VALUES
-- Year 1
('IT101', 'IT111', 'Intro to Cyber Security', 'DEPT003', 'LEVEL1', 'theoretical', 100, 1),
('IT102', 'IT112', 'IT Essentials', 'DEPT003', 'LEVEL1', 'practical', 100, 3),
('IT103', 'IT113', 'Technical English 1', 'DEPT003', 'LEVEL1', 'theoretical', 100, 3),
('IT104', 'IT114', 'Mathematics 1', 'DEPT003', 'LEVEL1', 'theoretical', 100, 3),
('IT105', 'IT115', 'Physics', 'DEPT003', 'LEVEL1', 'theoretical', 100, 3),
('IT106', 'IT116', 'Programming Essentials in Python', 'DEPT003', 'LEVEL1', 'practical', 100, 3),
('IT107', 'IT121', 'Programming Essentials in C', 'DEPT003', 'LEVEL1', 'practical', 100, 3),
('IT108', 'IT122', 'Cyber Security Essentials', 'DEPT003', 'LEVEL1', 'theoretical', 100, 2),
('IT109', 'IT123', 'Intro to IoT', 'DEPT003', 'LEVEL1', 'practical', 100, 3),
('IT110', 'IT124', 'MS Office', 'DEPT003', 'LEVEL1', 'practical', 100, 2),
('IT111', 'IT125', 'Technical English 2', 'DEPT003', 'LEVEL1', 'theoretical', 100, 3),
('IT112', 'IT126', 'Mathematics 2', 'DEPT003', 'LEVEL1', 'theoretical', 100, 3),
-- Year 2
('IT201', 'IT211', 'Linux Essentials', 'DEPT003', 'LEVEL2', 'practical', 100, 3),
('IT202', 'IT212', 'Programming Essentials in C++', 'DEPT003', 'LEVEL2', 'practical', 100, 3),
('IT203', 'IT213', 'Web Programming I', 'DEPT003', 'LEVEL2', 'practical', 100, 3),
('IT204', 'IT214', 'Introduction to DB', 'DEPT003', 'LEVEL2', 'theoretical', 100, 2),
('IT205', 'IT215', 'Digital Engineering', 'DEPT003', 'LEVEL2', 'practical', 100, 3),
('IT206', 'IT216', 'Operating System', 'DEPT003', 'LEVEL2', 'theoretical', 100, 2),
('IT207', 'IT221', 'Java Programming I', 'DEPT003', 'LEVEL2', 'practical', 100, 3),
('IT208', 'IT222', 'Web Programming II', 'DEPT003', 'LEVEL2', 'practical', 100, 3),
('IT209', 'IT223', 'CCNA Routing and Switching I', 'DEPT003', 'LEVEL2', 'practical', 100, 3),
('IT210', 'IT224', 'Data Structure', 'DEPT003', 'LEVEL2', 'practical', 100, 3),
('IT211B', 'IT225', 'Database Programming', 'DEPT003', 'LEVEL2', 'practical', 100, 2),
('IT212B', 'IT226', 'Capstone Design', 'DEPT003', 'LEVEL2', 'practical', 100, 2),
-- Year 3
('IT301', 'IT311', 'Advanced Programming in C', 'DEPT003', 'LEVEL3', 'practical', 100, 3),
('IT302', 'IT312', 'Data Communication', 'DEPT003', 'LEVEL3', 'theoretical', 100, 2),
('IT303', 'IT313', 'Java Programming II', 'DEPT003', 'LEVEL3', 'practical', 100, 3),
('IT304', 'IT314', 'Computer Architecture', 'DEPT003', 'LEVEL3', 'theoretical', 100, 2),
('IT305', 'IT315', 'Microprocessor', 'DEPT003', 'LEVEL3', 'practical', 100, 3),
('IT306', 'IT316', 'Computer Graphics', 'DEPT003', 'LEVEL3', 'practical', 100, 3),
('IT307', 'IT321', 'Advanced Programming in C++', 'DEPT003', 'LEVEL3', 'practical', 100, 3),
('IT308', 'IT322', 'Mobile Programming I', 'DEPT003', 'LEVEL3', 'practical', 100, 3),
('IT309', 'IT323', 'Embedded System', 'DEPT003', 'LEVEL3', 'practical', 100, 3),
('IT310', 'IT324', 'Network Programming', 'DEPT003', 'LEVEL3', 'practical', 100, 3),
('IT311B', 'IT325', 'Algorithm', 'DEPT003', 'LEVEL3', 'theoretical', 100, 2),
('IT312B', 'IT326', 'Software Engineering', 'DEPT003', 'LEVEL3', 'theoretical', 100, 2),
-- Year 4
('IT401', 'IT411', 'CCNA Routing and Switching II', 'DEPT003', 'LEVEL4', 'practical', 100, 3),
('IT402', 'IT412', 'Mobile Programming II', 'DEPT003', 'LEVEL4', 'practical', 100, 3),
('IT403', 'IT413', 'IoT Architecture and Protocols', 'DEPT003', 'LEVEL4', 'practical', 100, 3),
('IT404', 'IT414', 'Artificial Intelligence', 'DEPT003', 'LEVEL4', 'theoretical', 100, 2),
('IT405', 'IT415', 'Windows Programming I', 'DEPT003', 'LEVEL4', 'practical', 100, 3),
('IT406', 'IT416', 'Signal Processing', 'DEPT003', 'LEVEL4', 'theoretical', 100, 2),
('IT407', 'IT421', 'Robotics', 'DEPT003', 'LEVEL4', 'practical', 100, 3),
('IT408', 'IT422', 'Windows Programming II', 'DEPT003', 'LEVEL4', 'practical', 100, 3),
('IT409', 'IT423', 'Big Data and Analytics', 'DEPT003', 'LEVEL4', 'practical', 100, 2),
('IT410', 'IT424', 'IoT Security', 'DEPT003', 'LEVEL4', 'practical', 100, 3),
('IT411C', 'IT425', 'Machine Learning', 'DEPT003', 'LEVEL4', 'theoretical', 100, 2),
('IT412C', 'IT426', 'Entrepreneurship', 'DEPT003', 'LEVEL4', 'theoretical', 50, 1),
('IT413C', 'IT427', 'Graduation Project', 'DEPT003', 'LEVEL4', 'practical', 100, 2);

-- ============================================
-- COURSES - Mechatronics (DEPT002)
-- ============================================
INSERT INTO courses (id, code, name, department_id, academic_level_id, course_type, max_marks, credit_hours) VALUES
-- Year 1
('MEC101', 'MEC111', 'Technical English at Work Place I', 'DEPT002', 'LEVEL1', 'theoretical', 150, 3),
('MEC102', 'MEC112', 'Communication Skills in Technology', 'DEPT002', 'LEVEL1', 'practical', 150, 3),
('MEC103', 'MEC113', 'Mathematics for Technicians', 'DEPT002', 'LEVEL1', 'theoretical', 150, 3),
('MEC104', 'MEC114', 'Application of Math and Science', 'DEPT002', 'LEVEL1', 'theoretical', 150, 3),
('MEC105', 'MEC115', 'Health Safety and Risk Assessment', 'DEPT002', 'LEVEL1', 'practical', 100, 2),
('MEC106', 'MEC116', 'Physics for Technicians', 'DEPT002', 'LEVEL1', 'theoretical', 100, 2),
('MEC107', 'MEC121', 'Technical English at Work Place II', 'DEPT002', 'LEVEL1', 'theoretical', 150, 3),
('MEC108', 'MEC122', 'Environmental Studies', 'DEPT002', 'LEVEL1', 'theoretical', 50, 1),
('MEC109', 'MEC123', 'Basic Mechatronics Workshop', 'DEPT002', 'LEVEL1', 'practical', 150, 3),
('MEC110', 'MEC124', 'Computer Technology', 'DEPT002', 'LEVEL1', 'practical', 100, 2),
('MEC111B', 'MEC125', 'Electric Circuits', 'DEPT002', 'LEVEL1', 'practical', 150, 3),
('MEC112B', 'MEC126', 'CAD for Technicians', 'DEPT002', 'LEVEL1', 'practical', 100, 1),
('MEC113', 'MEC127', 'Computer-Aided Kinematics', 'DEPT002', 'LEVEL1', 'practical', 150, 3),
-- Year 2
('MEC201', 'MEC211', 'Pneumatics and Hydraulics Practice', 'DEPT002', 'LEVEL2', 'practical', 150, 3),
('MEC202', 'MEC212', 'Programming for Mechatronics', 'DEPT002', 'LEVEL2', 'practical', 150, 3),
('MEC203', 'MEC213', 'Material Selection for Applications', 'DEPT002', 'LEVEL2', 'practical', 150, 3),
('MEC204', 'MEC214', 'Fundamentals of PC-based Control', 'DEPT002', 'LEVEL2', 'practical', 150, 3),
('MEC205', 'MEC215', 'PLC Programming', 'DEPT002', 'LEVEL2', 'practical', 150, 4),
('MEC206', 'MEC221', 'Electrical and Electronics Workshop', 'DEPT002', 'LEVEL2', 'practical', 100, 1),
('MEC207', 'MEC222', 'Electromechanical Systems Maintenance', 'DEPT002', 'LEVEL2', 'practical', 150, 3),
('MEC208', 'MEC223', 'Capstone Design', 'DEPT002', 'LEVEL2', 'practical', 150, 3),
('MEC209', 'MEC224', 'Mechatronic Systems for Technologists', 'DEPT002', 'LEVEL2', 'practical', 150, 3),
('MEC210', 'MEC225', 'Selective Course I', 'DEPT002', 'LEVEL2', 'practical', 150, 3),
('MEC211B', 'MEC226', 'Enterprise and Entrepreneurship I', 'DEPT002', 'LEVEL2', 'practical', 150, 3),
-- Year 3
('MEC301', 'MEC311', 'Mathematics for Technologist', 'DEPT002', 'LEVEL3', 'theoretical', 150, 3),
('MEC302', 'MEC312', '3D CAD and Mechanical Modeling', 'DEPT002', 'LEVEL3', 'practical', 150, 3),
('MEC303', 'MEC313', 'Automatic Control', 'DEPT002', 'LEVEL3', 'practical', 150, 3),
('MEC304', 'MEC314', 'Robotic Systems', 'DEPT002', 'LEVEL3', 'practical', 150, 4),
('MEC305', 'MEC315', 'Sensors and Instrumentation', 'DEPT002', 'LEVEL3', 'practical', 150, 3),
('MEC306', 'MEC321', 'Power Electronics and Drives', 'DEPT002', 'LEVEL3', 'practical', 150, 3),
('MEC307', 'MEC322', 'CNC Technology', 'DEPT002', 'LEVEL3', 'practical', 150, 3),
('MEC308', 'MEC323', 'Mechanism Design for Technologists', 'DEPT002', 'LEVEL3', 'practical', 150, 3),
('MEC309', 'MEC324', 'Microprocessor Technology', 'DEPT002', 'LEVEL3', 'practical', 150, 4),
('MEC310', 'MEC325', 'Production System Planning', 'DEPT002', 'LEVEL3', 'practical', 150, 3),
-- Year 4
('MEC401', 'MEC411', 'Business Communication and Ethics', 'DEPT002', 'LEVEL4', 'theoretical', 50, 1),
('MEC402', 'MEC412', 'Embedded Systems', 'DEPT002', 'LEVEL4', 'practical', 150, 3),
('MEC403', 'MEC413', 'Digital Signal Processing', 'DEPT002', 'LEVEL4', 'practical', 150, 3),
('MEC404', 'MEC414', 'Dynamic Systems Modeling', 'DEPT002', 'LEVEL4', 'practical', 150, 3),
('MEC405', 'MEC415', 'Metrology and Quality Control', 'DEPT002', 'LEVEL4', 'practical', 150, 3),
('MEC406', 'MEC416', 'Human-Machine Interface Technology', 'DEPT002', 'LEVEL4', 'practical', 150, 3),
('MEC407', 'MEC421', 'Selective Course II', 'DEPT002', 'LEVEL4', 'practical', 150, 4),
('MEC408', 'MEC422', 'Enterprise and Entrepreneurship II', 'DEPT002', 'LEVEL4', 'practical', 150, 4),
('MEC409', 'MEC423', 'Advanced PLC', 'DEPT002', 'LEVEL4', 'practical', 150, 4),
('MEC410', 'MEC424', 'Graduation Project', 'DEPT002', 'LEVEL4', 'practical', 150, 4);

-- ============================================
-- SAMPLE STUDENTS
-- ============================================
INSERT INTO students (id, first_name, last_name, email, phone, gender, department_id, academic_level_id, password_hash) VALUES
('20241', 'Ahmed', 'Mohamed', 'ahmed@nctu.edu.eg', '01012345678', 'Male', 'DEPT003', 'LEVEL1', 'student123'),
('20242', 'Sara', 'Ali', 'sara@nctu.edu.eg', '01098765432', 'Female', 'DEPT003', 'LEVEL1', 'student123'),
('20243', 'Mahmoud', 'Hassan', 'mahmoud@nctu.edu.eg', '01112233445', 'Male', 'DEPT003', 'LEVEL2', 'student123'),
('20244', 'Nourhan', 'Ahmed', 'nourhan@nctu.edu.eg', '01223344556', 'Female', 'DEPT002', 'LEVEL1', 'student123'),
('20245', 'Yousef', 'Ibrahim', 'yousef@nctu.edu.eg', '01556677889', 'Male', 'DEPT002', 'LEVEL2', 'student123');

-- ============================================
-- SAMPLE PROFESSORS
-- ============================================
INSERT INTO professors (id, first_name, last_name, email, phone, department_id, specialization, password_hash) VALUES
('PROF001', 'Dr. Mohamed', 'Ibrahim', 'dr.mohamed@nctu.edu.eg', '01234567890', 'DEPT003', 'Software Engineering', 'prof123'),
('PROF002', 'Dr. Ahmed', 'ElSayed', 'dr.ahmed@nctu.edu.eg', '01234567891', 'DEPT002', 'Mechatronics', 'prof123'),
('PROF003', 'Dr. Fatma', 'Abdullah', 'dr.fatma@nctu.edu.eg', '01234567892', 'DEPT003', 'Networks and Security', 'prof123');

-- ============================================
-- VIEWS
-- ============================================

CREATE VIEW v_student_details AS
SELECT 
    s.id,
    s.first_name,
    s.last_name,
    CONCAT(s.first_name, ' ', s.last_name) AS full_name,
    s.email,
    s.phone,
    s.date_of_birth,
    s.gender,
    s.registration_date,
    s.academic_level_id,
    s.department_id,
    al.name AS academic_level,
    d.name AS department,
    c.name AS college
FROM students s
LEFT JOIN academic_levels al ON s.academic_level_id = al.id
LEFT JOIN departments d ON s.department_id = d.id
LEFT JOIN colleges c ON d.college_id = c.id;

CREATE VIEW v_professor_details AS
SELECT 
    p.id,
    p.first_name,
    p.last_name,
    CONCAT(p.first_name, ' ', p.last_name) AS full_name,
    p.email,
    p.phone,
    p.hire_date,
    p.specialization,
    p.department_id,
    d.name AS department,
    c.name AS college
FROM professors p
LEFT JOIN departments d ON p.department_id = d.id
LEFT JOIN colleges c ON d.college_id = c.id;

CREATE VIEW v_student_grades AS
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
    g.assignment1,
    g.assignment2,
    g.coursework,
    g.final_exam,
    g.total,
    g.percentage,
    g.gpa,
    g.evaluation,
    r.academic_year,
    r.semester,
    r.status
FROM registrations r
JOIN students s ON r.student_id = s.id
JOIN courses c ON r.course_id = c.id
LEFT JOIN grades g ON r.id = g.registration_id;

-- ============================================
-- STORED PROCEDURES
-- ============================================

DELIMITER //

CREATE PROCEDURE sp_calculate_grade(IN reg_id VARCHAR(20))
BEGIN
    DECLARE v_course_type VARCHAR(20);
    DECLARE v_max_marks INT;
    DECLARE v_ass1 DECIMAL(5,2);
    DECLARE v_ass2 DECIMAL(5,2);
    DECLARE v_cw DECIMAL(5,2);
    DECLARE v_final DECIMAL(5,2);
    DECLARE v_total DECIMAL(5,2);
    DECLARE v_percentage DECIMAL(5,2);
    DECLARE v_gpa DECIMAL(3,2);
    DECLARE v_evaluation VARCHAR(20);

    SELECT c.course_type, c.max_marks 
    INTO v_course_type, v_max_marks
    FROM registrations r
    JOIN courses c ON r.course_id = c.id
    WHERE r.id = reg_id;

    SELECT COALESCE(assignment1, 0), COALESCE(assignment2, 0), 
           COALESCE(coursework, 0), COALESCE(final_exam, 0)
    INTO v_ass1, v_ass2, v_cw, v_final
    FROM grades WHERE registration_id = reg_id;

    SET v_total = v_ass1 + v_ass2 + v_cw + v_final;
    SET v_percentage = (v_total / v_max_marks) * 100;

    IF v_percentage >= 85 THEN
        SET v_evaluation = 'Excellent';
        SET v_gpa = 4.0;
    ELSEIF v_percentage >= 75 THEN
        SET v_evaluation = 'Very Good';
        SET v_gpa = 3.0;
    ELSEIF v_percentage >= 65 THEN
        SET v_evaluation = 'Good';
        SET v_gpa = 2.0;
    ELSEIF v_percentage >= 60 THEN
        SET v_evaluation = 'Pass';
        SET v_gpa = 1.0;
    ELSE
        SET v_evaluation = 'Fail';
        SET v_gpa = 0.0;
    END IF;

    UPDATE grades 
    SET total = v_total,
        percentage = v_percentage,
        gpa = v_gpa,
        evaluation = v_evaluation
    WHERE registration_id = reg_id;
END //

CREATE PROCEDURE sp_calculate_student_cgpa(
    IN p_student_id VARCHAR(20),
    OUT p_cgpa DECIMAL(3,2)
)
BEGIN
    DECLARE total_points DECIMAL(10,2) DEFAULT 0;
    DECLARE total_credits INT DEFAULT 0;
    
    SELECT COALESCE(SUM(g.gpa * c.credit_hours), 0), COALESCE(SUM(c.credit_hours), 0)
    INTO total_points, total_credits
    FROM grades g
    JOIN registrations r ON g.registration_id = r.id
    JOIN courses c ON r.course_id = c.id
    WHERE r.student_id = p_student_id
    AND r.status = 'Completed';
    
    IF total_credits > 0 THEN
        SET p_cgpa = total_points / total_credits;
    ELSE
        SET p_cgpa = 0.00;
    END IF;
END //

DELIMITER ;

-- ============================================
-- END OF NCTU DATABASE SCHEMA
-- ============================================
