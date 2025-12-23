@echo off
setlocal enabledelayedexpansion

:: ============================================
:: Database Setup Script
:: NCTU Student Information System
:: ============================================

title NCTU SIS - Database Setup
color 0B

echo.
echo ========================================
echo   Database Setup Script
echo   NCTU Student Information System
echo ========================================
echo.

set "INSTALL_DIR=%~dp0"

:: Find MySQL
set "MYSQL_BIN="
if exist "C:\Program Files\MySQL\MySQL Server 8.0\bin\mysql.exe" set "MYSQL_BIN=C:\Program Files\MySQL\MySQL Server 8.0\bin"
if exist "C:\Program Files\MySQL\MySQL Server 8.4\bin\mysql.exe" set "MYSQL_BIN=C:\Program Files\MySQL\MySQL Server 8.4\bin"
if exist "C:\Program Files\MySQL\MySQL Server 9.0\bin\mysql.exe" set "MYSQL_BIN=C:\Program Files\MySQL\MySQL Server 9.0\bin"

if not defined MYSQL_BIN (
    echo [ERROR] MySQL not found!
    echo Please install MySQL Server first using setup.bat
    pause
    exit /b 1
)

echo [INFO] MySQL found at: %MYSQL_BIN%
echo.

:: Check schema files
echo Available schema files:
echo.
set "SCHEMA_COUNT=0"

if exist "%INSTALL_DIR%sql\schema_v3.sql" (
    set /a SCHEMA_COUNT+=1
    echo   !SCHEMA_COUNT!. schema_v3.sql (Latest with all features)
    set "SCHEMA_!SCHEMA_COUNT!=%INSTALL_DIR%sql\schema_v3.sql"
)

if exist "%INSTALL_DIR%sql\schema_v2.sql" (
    set /a SCHEMA_COUNT+=1
    echo   !SCHEMA_COUNT!. schema_v2.sql (RBAC version)
    set "SCHEMA_!SCHEMA_COUNT!=%INSTALL_DIR%sql\schema_v2.sql"
)

if exist "%INSTALL_DIR%sql\schema.sql" (
    set /a SCHEMA_COUNT+=1
    echo   !SCHEMA_COUNT!. schema.sql (Legacy - not recommended)
    set "SCHEMA_!SCHEMA_COUNT!=%INSTALL_DIR%sql\schema.sql"
)

if %SCHEMA_COUNT% equ 0 (
    echo [ERROR] No schema files found in sql\ directory!
    pause
    exit /b 1
)

echo.
set /p SCHEMA_CHOICE="Select schema (1-%SCHEMA_COUNT%): "

:: Validate choice
set "SELECTED_SCHEMA=!SCHEMA_%SCHEMA_CHOICE%!"
if not defined SELECTED_SCHEMA (
    echo [ERROR] Invalid selection!
    pause
    exit /b 1
)

echo.
echo [INFO] Selected: !SELECTED_SCHEMA!
echo.

:: Warning about existing data
echo ========================================
echo   WARNING: This will DROP and recreate
echo   the database. All existing data will
echo   be permanently deleted!
echo ========================================
echo.
set /p CONFIRM="Are you sure? (YES to continue): "

if /i not "%CONFIRM%"=="YES" (
    echo [INFO] Operation cancelled.
    pause
    exit /b 0
)

echo.
set /p MYSQL_USER="MySQL Username (default: root): "
if "%MYSQL_USER%"=="" set "MYSQL_USER=root"

set /p MYSQL_PASS="MySQL Password: "

echo.
echo [INFO] Executing schema...
echo.

"%MYSQL_BIN%\mysql.exe" -u %MYSQL_USER% -p%MYSQL_PASS% < "!SELECTED_SCHEMA!" 2>&1

if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo   Database Created Successfully!
    echo ========================================
    echo.
    echo Default login credentials:
    echo.
    echo   Super Admin:
    echo     Username: superadmin
    echo     Password: admin123
    echo.
    echo   Student Affairs:
    echo     Username: affairs  
    echo     Password: affairs123
    echo.
) else (
    echo.
    echo [ERROR] Failed to create database!
    echo.
    echo Please check:
    echo   1. MySQL service is running
    echo   2. Username and password are correct
    echo   3. User has CREATE DATABASE privileges
    echo.
)

pause
endlocal
exit /b 0
