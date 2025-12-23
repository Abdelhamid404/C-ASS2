# Student Information System (SIS)

A complete desktop application for managing student information, built with:
- **Backend**: C++17
- **Frontend**: HTML5, CSS3, JavaScript, Bootstrap 5
- **GUI Framework**: WebView
- **Database**: MySQL 8.0

## ?? Features

### For Administrators
- Dashboard with statistics (students, professors, courses, departments)
- Student Management (Add, Edit, Delete, View)
- Professor Management (Add, Edit, Delete, View)
- Course Management (Add, Edit, Delete, View)
- College & Department Management
- Lecture Hall & Laboratory Management
- Course Registration Management
- Grade Management
- Attendance Tracking

### For Professors
- View assigned courses
- Enter grades (Assignment 1, Assignment 2, Coursework, Final Exam)
- Record attendance
- View schedule

### For Students
- View personal information
- View grades and GPA
- View transcript
- View schedule
- View attendance records

## ?? Project Structure

```
SIS_Project/
??? main.cpp                    # Entry point - WebView setup
??? CMakeLists.txt              # Build configuration
??? README.md                   # This file
?
??? include/                    # Header files
?   ??? Database.h              # MySQL connection manager
?   ??? Student.h
?   ??? Professor.h
?   ??? Course.h
?   ??? Grade.h
?   ??? College.h
?   ??? Department.h
?   ??? AcademicLevel.h
?   ??? LectureHall.h
?   ??? Laboratory.h
?   ??? Section.h
?   ??? Schedule.h
?   ??? Registration.h
?   ??? Attendance.h
?   ??? JsonHelper.h
?
??? src/                        # Implementation files
?   ??? Database.cpp
?   ??? Student.cpp
?   ??? Professor.cpp
?   ??? Course.cpp
?   ??? Grade.cpp
?   ??? JsonHelper.cpp
?
??? web/                        # Frontend files
?   ??? index.html              # Login page
?   ??? dashboard.html          # Main dashboard
?   ??? students.html           # Student management
?   ??? my-grades.html          # Student grades view
?   ??? grades.html             # Grade entry
?   ??? css/
?   ?   ??? style.css           # Custom styles
?   ??? js/
?       ??? app.js              # Main JavaScript
?
??? lib/
?   ??? webview.h               # WebView library header
?
??? sql/
    ??? schema.sql              # Legacy schema (v1)
    ??? schema_v2.sql           # Current schema (v2 + RBAC)
```

## ??? Prerequisites

1. **Visual Studio 2022** with C++ development tools
2. **MySQL Server 8.0** - [Download](https://dev.mysql.com/downloads/mysql/)
3. **MySQL client headers + library** (comes with MySQL Server on Windows; needed for `mysql.h` / `libmysql`)
4. **Edge WebView2 Runtime** (required on Windows for WebView)
5. **CMake 3.16+** (optional, if building with CMake) - [Download](https://cmake.org/download/)

## ?? Installation

### Step 1: Set Up MySQL Database

1. Open MySQL Workbench or command line
2. Run the database schema (v2 + RBAC):
   ```sql
   source path/to/sql/schema_v2.sql;
   ```
   Or copy and paste the contents of `sql/schema_v2.sql`

> Note: `sql/schema.sql` is a legacy schema and will not work with the v2 RBAC application.

### Step 2: MySQL client library

This project uses the MySQL C API (`mysql.h` / `libmysql`).

- If you build with CMake on Windows, set `MYSQL_DIR` in `CMakeLists.txt` (or pass `-DMYSQL_DIR=...`) to your MySQL installation root (e.g. `C:\Program Files\MySQL\MySQL Server 8.0`).
- If you build with the `.sln`/`.vcxproj`, make sure your project has:
  - Include dir: `<MYSQL_DIR>\\include`
  - Link lib: `libmysql.lib` (from `<MYSQL_DIR>\\lib`)
  - Runtime DLL: `libmysql.dll` available in PATH or copied next to the built `.exe`

### Step 3: Get WebView Library

WebView is already included in `lib/`. On Windows, install **Edge WebView2 Runtime** if the window is blank.

### Step 4: Configure Database Connection

Edit `include/Database.h` and update the defaults in `Database::Database()`:
- `host`, `user`, `password`, `database`, `port`

### Step 5: Build the Project

Using CMake:
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -DMYSQL_DIR="C:/Program Files/MySQL/MySQL Server 8.0"
cmake --build . --config Release
```

Or using Visual Studio:
1. Open the project folder in Visual Studio 2022
2. Select Release configuration
3. Build > Build Solution (Ctrl+Shift+B)

### Step 6: Run the Application

1. Navigate to the build output directory
2. Run `SIS.exe`
3. The application will open in a native window

## ?? Default Login Credentials

| Role | Username/ID | Password |
|------|-------------|----------|
| Super Admin | superadmin | admin123 |
| Admin | admin | admin123 |
| Professor | PROF001 | prof123 |
| Student | 20241 | student123 |

## ?? Grade Calculation

Grades are calculated from the component maxima stored per course (`assignment1_max`, `assignment2_max`, `coursework_max`, `final_exam_max`) and `max_marks`.

### Evaluation Scale

| Percentage | Letter | Evaluation | GPA |
|------------|--------|------------|-----|
| >= 90% | A+ | Excellent | 4.0 |
| >= 85% | A | Excellent | 3.7 |
| >= 80% | B+ | Very Good | 3.3 |
| >= 75% | B | Very Good | 3.0 |
| >= 70% | C+ | Good | 2.7 |
| >= 65% | C | Good | 2.3 |
| >= 60% | D+ | Pass | 2.0 |
| >= 50% | D | Pass | 1.0 |
| < 50% | F | Fail | 0.0 |

## ?? Troubleshooting

### Cannot connect to database
1. Ensure MySQL server is running
2. Check username and password in Database.h
3. Verify the database `sis_database` exists
4. Check firewall settings for port 3306

### WebView not displaying
1. Ensure Edge WebView2 Runtime is installed
2. Check that web files are in the correct directory
3. Verify Bootstrap CDN is accessible

### Build errors
1. Ensure `MYSQL_DIR` points to a MySQL install that contains `include/` and `lib/`
2. Ensure `libmysql.dll` is available at runtime (PATH or copied next to the `.exe`)
3. Verify C++17 is enabled

## ?? License

This project is for educational purposes.

## ?? Authors

SIS Development Team

---

**Happy Coding! ??**
