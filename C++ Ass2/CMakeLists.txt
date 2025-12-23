# ============================================
# Student Information System (SIS)
# CMake Build Configuration
# ============================================
# 
# This file configures how to build the SIS project.
# 
# REQUIREMENTS:
# - CMake 3.16 or higher
# - Visual Studio 2022
# - MySQL client library (libmysql / mysqlclient)
# - WebView library
# 
# BUILD INSTRUCTIONS:
# 1. Create a build directory: mkdir build && cd build
# 2. Configure: cmake .. -G "Visual Studio 17 2022"
# 3. Build: cmake --build . --config Release
# ============================================

cmake_minimum_required(VERSION 3.16)

# Project name and version
project(SIS VERSION 1.0.0 LANGUAGES CXX)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================
# FIND REQUIRED PACKAGES
# ============================================

# Find MySQL client (C API) headers + library
# Set this to your MySQL installation root (Server or Connector/C).
set(MYSQL_DIR "C:/Program Files/MySQL/MySQL Server 8.0" CACHE PATH "MySQL installation root (contains include/ and lib/)")

find_path(MYSQL_INCLUDE_DIR
    NAMES mysql.h mysql/mysql.h
    HINTS "${MYSQL_DIR}/include" "${MYSQL_DIR}/include/mysql"
)

find_library(MYSQL_LIBRARY
    NAMES libmysql mysqlclient
    HINTS "${MYSQL_DIR}/lib" "${MYSQL_DIR}/lib64" "${MYSQL_DIR}/lib64/vs14" "${MYSQL_DIR}/lib64/vs16"
)

if(MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
    message(STATUS "Found MySQL include: ${MYSQL_INCLUDE_DIR}")
    message(STATUS "Found MySQL library: ${MYSQL_LIBRARY}")
else()
    message(WARNING "MySQL client library not found. Set MYSQL_DIR to your MySQL installation root.")
endif()

# ============================================
# SOURCE FILES
# ============================================

# Header files
set(HEADERS
    include/Database.h
    include/Student.h
    include/Professor.h
    include/Course.h
    include/Grade.h
    include/College.h
    include/Department.h
    include/AcademicLevel.h
    include/LectureHall.h
    include/Laboratory.h
    include/Section.h
    include/Schedule.h
    include/Registration.h
    include/Attendance.h
    include/JsonHelper.h
    include/SessionManager.h
)

# Source files
set(SOURCES
    main.cpp
    src/Database.cpp
    src/Student.cpp
    src/Professor.cpp
    src/Course.cpp
    src/Grade.cpp
    src/JsonHelper.cpp
    src/Attendance.cpp
    src/College.cpp
    src/Registration.cpp
    src/SessionManager.cpp
)

# ============================================
# CREATE EXECUTABLE
# ============================================

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# ============================================
# INCLUDE DIRECTORIES
# ============================================

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

if(MYSQL_INCLUDE_DIR)
    target_include_directories(${PROJECT_NAME} PRIVATE "${MYSQL_INCLUDE_DIR}")
endif()

# ============================================
# LINK LIBRARIES
# ============================================

# Windows-specific libraries
if(WIN32)
    if(NOT MYSQL_INCLUDE_DIR)
        message(FATAL_ERROR "MySQL headers not found. Set MYSQL_DIR to your MySQL installation root.")
    endif()
    if(NOT MYSQL_LIBRARY)
        message(FATAL_ERROR "MySQL client library not found. Set MYSQL_DIR to your MySQL installation root.")
    endif()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${MYSQL_LIBRARY}
        ws2_32
        advapi32
        user32
        shell32
        ole32
        oleaut32
        shlwapi
        version
    )
endif()

# ============================================
# COPY WEB FILES TO OUTPUT DIRECTORY
# ============================================

# Copy web folder to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/web
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/web
    COMMENT "Copying web files to output directory"
)

# Copy MySQL DLL to output directory (Windows) if available
if(WIN32)
    find_file(MYSQL_DLL
        NAMES libmysql.dll
        HINTS "${MYSQL_DIR}/bin" "${MYSQL_DIR}/lib" "${MYSQL_DIR}/lib64"
    )
    if(MYSQL_DLL)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MYSQL_DLL}"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Copying MySQL client DLL"
        )
    endif()
endif()

# ============================================
# COMPILER OPTIONS
# ============================================

if(MSVC)
    # MSVC specific options
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4           # Warning level 4
        /WX-          # Don't treat warnings as errors
        /MP           # Multi-processor compilation
        /utf-8        # UTF-8 source and execution charset
    )
    
    # Disable specific warnings
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS
        _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
    )
endif()

# ============================================
# INSTALL RULES
# ============================================

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

install(DIRECTORY web/
    DESTINATION bin/web
)

install(DIRECTORY sql/
    DESTINATION sql
)

# ============================================
# PRINT CONFIGURATION SUMMARY
# ============================================

message(STATUS "")
message(STATUS "========================================")
message(STATUS "SIS Build Configuration Summary")
message(STATUS "========================================")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "MySQL_DIR: ${MYSQL_DIR}")
message(STATUS "Output directory: ${CMAKE_BINARY_DIR}")
message(STATUS "========================================")
message(STATUS "")
