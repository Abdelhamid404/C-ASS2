-- ================================================================================
-- NCTU Student Information System - Sample Data
-- Run this AFTER schema_v3.sql
-- ================================================================================

USE nctu_sis;

-- ================================================================================
-- SAMPLE PROFESSORS (10 professors across departments)
-- ================================================================================

-- Create user accounts for professors (username = professor ID for easy login)
INSERT INTO users (username, password_hash, full_name, email, phone, role_id) VALUES
('PROF001', 'prof123', 'Dr. Mohamed Ibrahim', 'mohamed.ibrahim@nctu.edu.eg', '01012345001', 'ROLE_PROFESSOR'),
('PROF002', 'prof123', 'Dr. Ahmed Hassan', 'ahmed.hassan@nctu.edu.eg', '01012345002', 'ROLE_PROFESSOR'),
('PROF003', 'prof123', 'Dr. Sara Ali', 'sara.ali@nctu.edu.eg', '01012345003', 'ROLE_PROFESSOR'),
('PROF004', 'prof123', 'Dr. Fatma Abdullah', 'fatma.abdullah@nctu.edu.eg', '01012345004', 'ROLE_PROFESSOR'),
('PROF005', 'prof123', 'Dr. Khaled Mostafa', 'khaled.mostafa@nctu.edu.eg', '01012345005', 'ROLE_PROFESSOR'),
('PROF006', 'prof123', 'Dr. Mona ElSayed', 'mona.elsayed@nctu.edu.eg', '01012345006', 'ROLE_PROFESSOR'),
('PROF007', 'prof123', 'Eng. Omar Youssef', 'omar.youssef@nctu.edu.eg', '01012345007', 'ROLE_PROFESSOR'),
('PROF008', 'prof123', 'Dr. Nadia Mahmoud', 'nadia.mahmoud@nctu.edu.eg', '01012345008', 'ROLE_PROFESSOR'),
('PROF009', 'prof123', 'Dr. Tarek Samir', 'tarek.samir@nctu.edu.eg', '01012345009', 'ROLE_PROFESSOR'),
('PROF010', 'prof123', 'Dr. Heba Adel', 'heba.adel@nctu.edu.eg', '01012345010', 'ROLE_PROFESSOR');

-- Create professor records linked to users
INSERT INTO professors (id, user_id, title, first_name, last_name, email, phone, department_id, specialization) VALUES
('PROF001', (SELECT id FROM users WHERE username='PROF001'), 'Dr.', 'Mohamed', 'Ibrahim', 'mohamed.ibrahim@nctu.edu.eg', '01012345001', 'DEPT003', 'Cybersecurity & Networks'),
('PROF002', (SELECT id FROM users WHERE username='PROF002'), 'Dr.', 'Ahmed', 'Hassan', 'ahmed.hassan@nctu.edu.eg', '01012345002', 'DEPT001', 'Solar Energy Systems'),
('PROF003', (SELECT id FROM users WHERE username='PROF003'), 'Dr.', 'Sara', 'Ali', 'sara.ali@nctu.edu.eg', '01012345003', 'DEPT002', 'Robotics & Automation'),
('PROF004', (SELECT id FROM users WHERE username='PROF004'), 'Dr.', 'Fatma', 'Abdullah', 'fatma.abdullah@nctu.edu.eg', '01012345004', 'DEPT003', 'Software Engineering'),
('PROF005', (SELECT id FROM users WHERE username='PROF005'), 'Dr.', 'Khaled', 'Mostafa', 'khaled.mostafa@nctu.edu.eg', '01012345005', 'DEPT004', 'Automotive Electronics'),
('PROF006', (SELECT id FROM users WHERE username='PROF006'), 'Dr.', 'Mona', 'ElSayed', 'mona.elsayed@nctu.edu.eg', '01012345006', 'DEPT005', 'Petroleum Processing'),
('PROF007', (SELECT id FROM users WHERE username='PROF007'), 'Eng.', 'Omar', 'Youssef', 'omar.youssef@nctu.edu.eg', '01012345007', 'DEPT002', 'PLC & Control Systems'),
('PROF008', (SELECT id FROM users WHERE username='PROF008'), 'Dr.', 'Nadia', 'Mahmoud', 'nadia.mahmoud@nctu.edu.eg', '01012345008', 'DEPT001', 'Wind Energy'),
('PROF009', (SELECT id FROM users WHERE username='PROF009'), 'Dr.', 'Tarek', 'Samir', 'tarek.samir@nctu.edu.eg', '01012345009', 'DEPT003', 'Artificial Intelligence'),
('PROF010', (SELECT id FROM users WHERE username='PROF010'), 'Dr.', 'Heba', 'Adel', 'heba.adel@nctu.edu.eg', '01012345010', 'DEPT004', 'Engine Management Systems');

-- ================================================================================
-- SAMPLE STUDENTS (20 students across departments and levels)
-- ================================================================================

-- Create user accounts for students
INSERT INTO users (username, password_hash, full_name, email, phone, role_id) VALUES
-- IT Department Students
('2024001', 'student123', 'Ahmed Mohamed Ali', 'ahmed.ali@student.nctu.edu.eg', '01112340001', 'ROLE_STUDENT'),
('2024002', 'student123', 'Sara Ibrahim Hassan', 'sara.hassan@student.nctu.edu.eg', '01112340002', 'ROLE_STUDENT'),
('2024003', 'student123', 'Mahmoud Khaled Omar', 'mahmoud.omar@student.nctu.edu.eg', '01112340003', 'ROLE_STUDENT'),
('2024004', 'student123', 'Nourhan Ahmed Youssef', 'nourhan.youssef@student.nctu.edu.eg', '01112340004', 'ROLE_STUDENT'),
-- Mechatronics Students
('2024005', 'student123', 'Yousef Ibrahim Mostafa', 'yousef.mostafa@student.nctu.edu.eg', '01112340005', 'ROLE_STUDENT'),
('2024006', 'student123', 'Mariam Ali Samir', 'mariam.samir@student.nctu.edu.eg', '01112340006', 'ROLE_STUDENT'),
('2024007', 'student123', 'Omar Hassan Abdullah', 'omar.abdullah@student.nctu.edu.eg', '01112340007', 'ROLE_STUDENT'),
-- Renewable Energy Students
('2024008', 'student123', 'Fatma Mohamed Adel', 'fatma.adel@student.nctu.edu.eg', '01112340008', 'ROLE_STUDENT'),
('2024009', 'student123', 'Khaled Ahmed Mahmoud', 'khaled.mahmoud@student.nctu.edu.eg', '01112340009', 'ROLE_STUDENT'),
('2024010', 'student123', 'Heba Ibrahim ElSayed', 'heba.elsayed@student.nctu.edu.eg', '01112340010', 'ROLE_STUDENT'),
-- Autotronics Students
('2024011', 'student123', 'Tarek Youssef Ali', 'tarek.ali@student.nctu.edu.eg', '01112340011', 'ROLE_STUDENT'),
('2024012', 'student123', 'Mona Hassan Omar', 'mona.omar@student.nctu.edu.eg', '01112340012', 'ROLE_STUDENT'),
('2024013', 'student123', 'Ali Mohamed Khaled', 'ali.khaled@student.nctu.edu.eg', '01112340013', 'ROLE_STUDENT'),
-- Petroleum Students
('2024014', 'student123', 'Nadia Mahmoud Hassan', 'nadia.hassan@student.nctu.edu.eg', '01112340014', 'ROLE_STUDENT'),
('2024015', 'student123', 'Samir Ibrahim Ali', 'samir.ali@student.nctu.edu.eg', '01112340015', 'ROLE_STUDENT'),
-- Year 2 Students
('2023001', 'student123', 'Adel Mohamed Youssef', 'adel.youssef@student.nctu.edu.eg', '01112340016', 'ROLE_STUDENT'),
('2023002', 'student123', 'Rania Ahmed Hassan', 'rania.hassan@student.nctu.edu.eg', '01112340017', 'ROLE_STUDENT'),
-- Year 3 Students
('2022001', 'student123', 'Hassan Omar Mahmoud', 'hassan.mahmoud@student.nctu.edu.eg', '01112340018', 'ROLE_STUDENT'),
('2022002', 'student123', 'Laila Ibrahim Ali', 'laila.ali@student.nctu.edu.eg', '01112340019', 'ROLE_STUDENT'),
-- Year 4 Student
('2021001', 'student123', 'Mohamed Tarek Samir', 'mohamed.samir@student.nctu.edu.eg', '01112340020', 'ROLE_STUDENT');

-- Create student records linked to users
INSERT INTO students (id, user_id, first_name, last_name, date_of_birth, email, phone, gender, department_id, academic_level_id) VALUES
-- IT Department - Year 1
('2024001', (SELECT id FROM users WHERE username='2024001'), 'Ahmed', 'Ali', '2006-03-15', 'ahmed.ali@student.nctu.edu.eg', '01112340001', 'male', 'DEPT003', 'LEVEL1'),
('2024002', (SELECT id FROM users WHERE username='2024002'), 'Sara', 'Hassan', '2006-07-22', 'sara.hassan@student.nctu.edu.eg', '01112340002', 'female', 'DEPT003', 'LEVEL1'),
('2024003', (SELECT id FROM users WHERE username='2024003'), 'Mahmoud', 'Omar', '2006-01-10', 'mahmoud.omar@student.nctu.edu.eg', '01112340003', 'male', 'DEPT003', 'LEVEL1'),
('2024004', (SELECT id FROM users WHERE username='2024004'), 'Nourhan', 'Youssef', '2006-09-05', 'nourhan.youssef@student.nctu.edu.eg', '01112340004', 'female', 'DEPT003', 'LEVEL1'),
-- Mechatronics - Year 1
('2024005', (SELECT id FROM users WHERE username='2024005'), 'Yousef', 'Mostafa', '2006-04-18', 'yousef.mostafa@student.nctu.edu.eg', '01112340005', 'male', 'DEPT002', 'LEVEL1'),
('2024006', (SELECT id FROM users WHERE username='2024006'), 'Mariam', 'Samir', '2006-11-28', 'mariam.samir@student.nctu.edu.eg', '01112340006', 'female', 'DEPT002', 'LEVEL1'),
('2024007', (SELECT id FROM users WHERE username='2024007'), 'Omar', 'Abdullah', '2006-06-12', 'omar.abdullah@student.nctu.edu.eg', '01112340007', 'male', 'DEPT002', 'LEVEL1'),
-- Renewable Energy - Year 1
('2024008', (SELECT id FROM users WHERE username='2024008'), 'Fatma', 'Adel', '2006-02-25', 'fatma.adel@student.nctu.edu.eg', '01112340008', 'female', 'DEPT001', 'LEVEL1'),
('2024009', (SELECT id FROM users WHERE username='2024009'), 'Khaled', 'Mahmoud', '2006-08-08', 'khaled.mahmoud@student.nctu.edu.eg', '01112340009', 'male', 'DEPT001', 'LEVEL1'),
('2024010', (SELECT id FROM users WHERE username='2024010'), 'Heba', 'ElSayed', '2006-12-03', 'heba.elsayed@student.nctu.edu.eg', '01112340010', 'female', 'DEPT001', 'LEVEL1'),
-- Autotronics - Year 1
('2024011', (SELECT id FROM users WHERE username='2024011'), 'Tarek', 'Ali', '2006-05-20', 'tarek.ali@student.nctu.edu.eg', '01112340011', 'male', 'DEPT004', 'LEVEL1'),
('2024012', (SELECT id FROM users WHERE username='2024012'), 'Mona', 'Omar', '2006-10-14', 'mona.omar@student.nctu.edu.eg', '01112340012', 'female', 'DEPT004', 'LEVEL1'),
('2024013', (SELECT id FROM users WHERE username='2024013'), 'Ali', 'Khaled', '2006-07-30', 'ali.khaled@student.nctu.edu.eg', '01112340013', 'male', 'DEPT004', 'LEVEL1'),
-- Petroleum - Year 1
('2024014', (SELECT id FROM users WHERE username='2024014'), 'Nadia', 'Hassan', '2006-03-08', 'nadia.hassan@student.nctu.edu.eg', '01112340014', 'female', 'DEPT005', 'LEVEL1'),
('2024015', (SELECT id FROM users WHERE username='2024015'), 'Samir', 'Ali', '2006-09-22', 'samir.ali@student.nctu.edu.eg', '01112340015', 'male', 'DEPT005', 'LEVEL1'),
-- Year 2 Students
('2023001', (SELECT id FROM users WHERE username='2023001'), 'Adel', 'Youssef', '2005-04-12', 'adel.youssef@student.nctu.edu.eg', '01112340016', 'male', 'DEPT003', 'LEVEL2'),
('2023002', (SELECT id FROM users WHERE username='2023002'), 'Rania', 'Hassan', '2005-08-25', 'rania.hassan@student.nctu.edu.eg', '01112340017', 'female', 'DEPT002', 'LEVEL2'),
-- Year 3 Students
('2022001', (SELECT id FROM users WHERE username='2022001'), 'Hassan', 'Mahmoud', '2004-01-30', 'hassan.mahmoud@student.nctu.edu.eg', '01112340018', 'male', 'DEPT003', 'LEVEL3'),
('2022002', (SELECT id FROM users WHERE username='2022002'), 'Laila', 'Ali', '2004-06-18', 'laila.ali@student.nctu.edu.eg', '01112340019', 'female', 'DEPT001', 'LEVEL3'),
-- Year 4 Student
('2021001', (SELECT id FROM users WHERE username='2021001'), 'Mohamed', 'Samir', '2003-11-05', 'mohamed.samir@student.nctu.edu.eg', '01112340020', 'male', 'DEPT003', 'LEVEL4');

-- ================================================================================
-- LECTURE HALLS (8 halls)
-- ================================================================================

INSERT INTO lecture_halls (id, name, building, floor, max_capacity, seating_desc, ac_units, fan_units, lighting_units, description) VALUES
('HALL-A101', 'Hall A101', 'Building A', 1, 120, 'Theater seating', 4, 0, 24, 'Main lecture hall with projector and sound system'),
('HALL-A102', 'Hall A102', 'Building A', 1, 80, 'Theater seating', 3, 0, 16, 'Medium lecture hall'),
('HALL-A201', 'Hall A201', 'Building A', 2, 60, 'Classroom seating', 2, 2, 12, 'Standard classroom'),
('HALL-A202', 'Hall A202', 'Building A', 2, 60, 'Classroom seating', 2, 2, 12, 'Standard classroom'),
('HALL-B101', 'Hall B101', 'Building B', 1, 100, 'Theater seating', 3, 0, 20, 'Large lecture hall'),
('HALL-B102', 'Hall B102', 'Building B', 1, 50, 'Classroom seating', 2, 2, 10, 'Small classroom'),
('HALL-B201', 'Hall B201', 'Building B', 2, 40, 'Seminar seating', 2, 0, 8, 'Seminar room'),
('HALL-C101', 'Auditorium', 'Building C', 1, 300, 'Auditorium seating', 8, 0, 60, 'Main auditorium for events and conferences');

-- ================================================================================
-- LABORATORIES (6 labs)
-- ================================================================================

INSERT INTO laboratories (id, name, building, floor, max_capacity, lab_type, computers_count, seats_count, ac_units, fan_units, lighting_units, description) VALUES
('LAB-IT01', 'Computer Lab 1', 'IT Building', 1, 40, 'Computer', 40, 40, 3, 0, 16, 'General purpose computer lab with high-spec PCs'),
('LAB-IT02', 'Computer Lab 2', 'IT Building', 1, 35, 'Computer', 35, 35, 3, 0, 14, 'Programming and development lab'),
('LAB-IT03', 'Networking Lab', 'IT Building', 2, 25, 'Networking', 20, 25, 2, 0, 10, 'Cisco networking equipment and routers'),
('LAB-MEC01', 'Mechatronics Lab', 'Engineering Building', 1, 30, 'Mechatronics', 15, 30, 2, 2, 12, 'PLC, robotics, and automation equipment'),
('LAB-REN01', 'Renewable Energy Lab', 'Engineering Building', 2, 25, 'Energy', 10, 25, 2, 2, 10, 'Solar panels, inverters, and measurement equipment'),
('LAB-AUT01', 'Autotronics Lab', 'Workshop Building', 1, 20, 'Automotive', 10, 20, 2, 4, 8, 'Vehicle diagnostics and engine systems');

-- ================================================================================
-- COURSE ASSIGNMENTS (Professors to Courses for Fall 2025)
-- ================================================================================

INSERT INTO course_assignments (course_id, professor_id, semester_id, is_primary, assigned_by) VALUES
-- IT Department - Prof Mohamed (Cybersecurity)
('IT-Y1S1-01', 'PROF001', '2025-1', TRUE, 1),  -- Intro to Cyber Security
('IT-Y1S2-02', 'PROF001', '2025-1', TRUE, 1),  -- Cyber Security Essentials
-- IT Department - Prof Fatma (Software)
('IT-Y1S1-06', 'PROF004', '2025-1', TRUE, 1),  -- Programming Essentials in Python
('IT-Y2S1-02', 'PROF004', '2025-1', TRUE, 1),  -- Programming Essentials in C++
-- IT Department - Prof Tarek (AI)
('IT-Y4S1-04', 'PROF009', '2025-1', TRUE, 1),  -- Artificial Intelligence
('IT-Y4S2-05', 'PROF009', '2025-1', TRUE, 1),  -- Machine Learning
-- Mechatronics - Prof Sara (Robotics)
('MEC-Y3S1-04', 'PROF003', '2025-1', TRUE, 1),  -- Robotic Systems
('MEC-Y4S1-02', 'PROF003', '2025-1', TRUE, 1),  -- Embedded Systems
-- Mechatronics - Prof Omar (PLC)
('MEC-Y2S1-05', 'PROF007', '2025-1', TRUE, 1),  -- PLC
('MEC-Y4S2-01', 'PROF007', '2025-1', TRUE, 1),  -- Advanced PLC
-- Renewable Energy - Prof Ahmed (Solar)
('REN-Y1S1-03', 'PROF002', '2025-1', TRUE, 1),  -- Renewable Energy
('REN-Y3S1-02', 'PROF002', '2025-1', TRUE, 1),  -- Solar Energy and Applications
-- Renewable Energy - Prof Nadia (Wind)
('REN-Y2S1-03', 'PROF008', '2025-1', TRUE, 1),  -- Renewable Energy Technology
('REN-Y4S2-02', 'PROF008', '2025-1', TRUE, 1),  -- Smart Grid Technology
-- Autotronics - Prof Khaled
('AUT-Y3S1-03', 'PROF005', '2025-1', TRUE, 1),  -- Electronic Ignition Circuits
('AUT-Y4S1-02', 'PROF005', '2025-1', TRUE, 1),  -- Electronic Engine Control Systems
-- Autotronics - Prof Heba
('AUT-Y1S2-05', 'PROF010', '2025-1', TRUE, 1),  -- Car Engines 1
('AUT-Y2S1-04', 'PROF010', '2025-1', TRUE, 1),  -- Car Engines 2
-- Petroleum - Prof Mona
('PET-Y1S1-07', 'PROF006', '2025-1', TRUE, 1),  -- Introduction to Petroleum Industry
('PET-Y2S1-03', 'PROF006', '2025-1', TRUE, 1);  -- Technology of Oil and Gas Plants

-- ================================================================================
-- SAMPLE REGISTRATIONS (Register students to their courses)
-- ================================================================================

-- IT Year 1 Students - Semester 1 Courses
INSERT INTO registrations (student_id, course_id, semester_id, registered_by) VALUES
('2024001', 'IT-Y1S1-01', '2025-1', 2),
('2024001', 'IT-Y1S1-02', '2025-1', 2),
('2024001', 'IT-Y1S1-03', '2025-1', 2),
('2024001', 'IT-Y1S1-04', '2025-1', 2),
('2024001', 'IT-Y1S1-05', '2025-1', 2),
('2024001', 'IT-Y1S1-06', '2025-1', 2),
('2024002', 'IT-Y1S1-01', '2025-1', 2),
('2024002', 'IT-Y1S1-02', '2025-1', 2),
('2024002', 'IT-Y1S1-03', '2025-1', 2),
('2024002', 'IT-Y1S1-04', '2025-1', 2),
('2024002', 'IT-Y1S1-05', '2025-1', 2),
('2024002', 'IT-Y1S1-06', '2025-1', 2),
('2024003', 'IT-Y1S1-01', '2025-1', 2),
('2024003', 'IT-Y1S1-02', '2025-1', 2),
('2024003', 'IT-Y1S1-03', '2025-1', 2),
('2024003', 'IT-Y1S1-04', '2025-1', 2),
('2024003', 'IT-Y1S1-05', '2025-1', 2),
('2024003', 'IT-Y1S1-06', '2025-1', 2);

-- IT Year 2 Student
INSERT INTO registrations (student_id, course_id, semester_id, registered_by) VALUES
('2023001', 'IT-Y2S1-01', '2025-1', 2),
('2023001', 'IT-Y2S1-02', '2025-1', 2),
('2023001', 'IT-Y2S1-03', '2025-1', 2),
('2023001', 'IT-Y2S1-04', '2025-1', 2),
('2023001', 'IT-Y2S1-05', '2025-1', 2),
('2023001', 'IT-Y2S1-06', '2025-1', 2);

-- Mechatronics Year 1 Students
INSERT INTO registrations (student_id, course_id, semester_id, registered_by) VALUES
('2024005', 'MEC-Y1S1-01', '2025-1', 2),
('2024005', 'MEC-Y1S1-02', '2025-1', 2),
('2024005', 'MEC-Y1S1-03', '2025-1', 2),
('2024005', 'MEC-Y1S1-04', '2025-1', 2),
('2024005', 'MEC-Y1S1-05', '2025-1', 2),
('2024005', 'MEC-Y1S1-06', '2025-1', 2),
('2024006', 'MEC-Y1S1-01', '2025-1', 2),
('2024006', 'MEC-Y1S1-02', '2025-1', 2),
('2024006', 'MEC-Y1S1-03', '2025-1', 2),
('2024006', 'MEC-Y1S1-04', '2025-1', 2),
('2024006', 'MEC-Y1S1-05', '2025-1', 2),
('2024006', 'MEC-Y1S1-06', '2025-1', 2);

-- Renewable Energy Year 1 Students
INSERT INTO registrations (student_id, course_id, semester_id, registered_by) VALUES
('2024008', 'REN-Y1S1-01', '2025-1', 2),
('2024008', 'REN-Y1S1-02', '2025-1', 2),
('2024008', 'REN-Y1S1-03', '2025-1', 2),
('2024008', 'REN-Y1S1-04', '2025-1', 2),
('2024008', 'REN-Y1S1-05', '2025-1', 2),
('2024008', 'REN-Y1S1-06', '2025-1', 2);

-- ================================================================================
-- CREATE GRADE RECORDS FOR ALL REGISTRATIONS
-- This is required so professors can enter/update grades
-- ================================================================================
INSERT INTO grades (registration_id)
SELECT id FROM registrations;

SELECT 'Sample data inserted successfully!' AS status;
SELECT CONCAT('Students: ', COUNT(*)) AS count FROM students;
SELECT CONCAT('Professors: ', COUNT(*)) AS count FROM professors;
SELECT CONCAT('Lecture Halls: ', COUNT(*)) AS count FROM lecture_halls;
SELECT CONCAT('Laboratories: ', COUNT(*)) AS count FROM laboratories;
SELECT CONCAT('Course Assignments: ', COUNT(*)) AS count FROM course_assignments;
SELECT CONCAT('Registrations: ', COUNT(*)) AS count FROM registrations;



