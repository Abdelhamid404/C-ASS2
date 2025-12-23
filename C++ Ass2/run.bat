@echo off
setlocal

:: ============================================
:: Quick Start Script
:: NCTU Student Information System
:: ============================================

title NCTU SIS - Starting...
color 0A

set "APP_DIR=%~dp0"

echo.
echo ========================================
echo   Starting Student Information System
echo ========================================
echo.

:: Check if executable exists
set "EXE_PATH="

:: Check common build output locations
if exist "%APP_DIR%x64\Release\C++ Ass2.exe" (
    set "EXE_PATH=%APP_DIR%x64\Release\C++ Ass2.exe"
    set "EXE_DIR=%APP_DIR%x64\Release"
)
if exist "%APP_DIR%x64\Debug\C++ Ass2.exe" (
    set "EXE_PATH=%APP_DIR%x64\Debug\C++ Ass2.exe"
    set "EXE_DIR=%APP_DIR%x64\Debug"
)
if exist "%APP_DIR%Release\C++ Ass2.exe" (
    set "EXE_PATH=%APP_DIR%Release\C++ Ass2.exe"
    set "EXE_DIR=%APP_DIR%Release"
)
if exist "%APP_DIR%Debug\C++ Ass2.exe" (
    set "EXE_PATH=%APP_DIR%Debug\C++ Ass2.exe"
    set "EXE_DIR=%APP_DIR%Debug"
)
if exist "%APP_DIR%build\Release\C++ Ass2.exe" (
    set "EXE_PATH=%APP_DIR%build\Release\C++ Ass2.exe"
    set "EXE_DIR=%APP_DIR%build\Release"
)

if not defined EXE_PATH (
    echo [ERROR] Application executable not found!
    echo.
    echo Please build the project first:
    echo   1. Open the solution in Visual Studio 2022
    echo   2. Select Release or Debug configuration
    echo   3. Build ^> Build Solution (Ctrl+Shift+B)
    echo.
    pause
    exit /b 1
)

echo [INFO] Found executable: %EXE_PATH%

:: Check for libmysql.dll in exe directory
if not exist "%EXE_DIR%\libmysql.dll" (
    echo [INFO] Copying libmysql.dll...
    
    :: Try to find and copy libmysql.dll
    if exist "%APP_DIR%libmysql.dll" (
        copy /Y "%APP_DIR%libmysql.dll" "%EXE_DIR%\" >nul
    ) else if exist "C:\Program Files\MySQL\MySQL Server 8.0\lib\libmysql.dll" (
        copy /Y "C:\Program Files\MySQL\MySQL Server 8.0\lib\libmysql.dll" "%EXE_DIR%\" >nul
    ) else if exist "C:\Program Files\MySQL\MySQL Server 8.4\lib\libmysql.dll" (
        copy /Y "C:\Program Files\MySQL\MySQL Server 8.4\lib\libmysql.dll" "%EXE_DIR%\" >nul
    ) else (
        echo [WARNING] libmysql.dll not found. Application may not start.
    )
)

:: Copy web folder if needed
if not exist "%EXE_DIR%\web" (
    echo [INFO] Copying web files...
    xcopy /E /I /Y "%APP_DIR%web" "%EXE_DIR%\web" >nul 2>&1
)

:: Check MySQL service
echo [INFO] Checking MySQL service...
sc query MySQL80 >nul 2>&1
if %errorlevel% neq 0 (
    sc query MySQL >nul 2>&1
    if %errorlevel% neq 0 (
        echo [WARNING] MySQL service may not be running!
        echo           Make sure MySQL is started before using the app.
    )
)

echo.
echo [INFO] Starting application...
echo.

:: Change to exe directory and run
cd /d "%EXE_DIR%"
start "" "%EXE_PATH%"

echo [OK] Application started!
echo.

timeout /t 3 >nul
exit /b 0
