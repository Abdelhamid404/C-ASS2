@echo off
setlocal enabledelayedexpansion

:: ============================================
:: Student Information System - Setup Script
:: NCTU - Windows 10/11 Dependency Installer
:: ============================================

title NCTU SIS - Setup Script
color 0A

echo.
echo ========================================
echo   Student Information System v2.0
echo   Dependency Installation Script
echo   NCTU - Windows 10/11
echo ========================================
echo.

:: Check for Administrator privileges
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] This script requires Administrator privileges.
    echo.
    echo Please right-click on setup.bat and select "Run as administrator"
    echo.
    pause
    exit /b 1
)

echo [INFO] Running with Administrator privileges...
echo.

:: Set installation directory
set "INSTALL_DIR=%~dp0"
set "TOOLS_DIR=%INSTALL_DIR%tools"
set "MYSQL_INSTALLER_URL=https://dev.mysql.com/get/Downloads/MySQLInstaller/mysql-installer-community-8.0.36.0.msi"
set "WEBVIEW2_URL=https://go.microsoft.com/fwlink/p/?LinkId=2124703"
set "VCREDIST_URL=https://aka.ms/vs/17/release/vc_redist.x64.exe"

:: Create tools directory
if not exist "%TOOLS_DIR%" mkdir "%TOOLS_DIR%"

echo [STEP 1/6] Checking system requirements...
echo.

:: Check Windows version
for /f "tokens=4-5 delims=. " %%i in ('ver') do set VERSION=%%i.%%j
echo [INFO] Windows Version: %VERSION%

:: Check if 64-bit
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    echo [INFO] Architecture: 64-bit
    set "ARCH=x64"
) else (
    echo [INFO] Architecture: 32-bit
    set "ARCH=x86"
)

echo.
echo ========================================
echo   Installing Dependencies
echo ========================================
echo.

:: ----------------------------------------
:: STEP 2: Install Visual C++ Redistributable
:: ----------------------------------------
echo [STEP 2/6] Installing Visual C++ Redistributable 2022...

:: Check if already installed
reg query "HKLM\SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" >nul 2>&1
if %errorlevel% equ 0 (
    echo [INFO] Visual C++ Redistributable already installed. Skipping...
) else (
    echo [INFO] Downloading Visual C++ Redistributable...
    powershell -Command "& {[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri '%VCREDIST_URL%' -OutFile '%TOOLS_DIR%\vc_redist.x64.exe'}" 2>nul
    
    if exist "%TOOLS_DIR%\vc_redist.x64.exe" (
        echo [INFO] Installing Visual C++ Redistributable...
        "%TOOLS_DIR%\vc_redist.x64.exe" /install /quiet /norestart
        echo [OK] Visual C++ Redistributable installed successfully.
    ) else (
        echo [WARNING] Could not download VC++ Redistributable. Please install manually.
    )
)
echo.

:: ----------------------------------------
:: STEP 3: Install Microsoft Edge WebView2 Runtime
:: ----------------------------------------
echo [STEP 3/6] Installing Microsoft Edge WebView2 Runtime...

:: Check if WebView2 is already installed
reg query "HKLM\SOFTWARE\WOW6432Node\Microsoft\EdgeUpdate\Clients\{F3017226-FE2A-4295-8BDF-00C3A9A7E4C5}" >nul 2>&1
if %errorlevel% equ 0 (
    echo [INFO] WebView2 Runtime already installed. Skipping...
) else (
    reg query "HKLM\SOFTWARE\Microsoft\EdgeUpdate\Clients\{F3017226-FE2A-4295-8BDF-00C3A9A7E4C5}" >nul 2>&1
    if %errorlevel% equ 0 (
        echo [INFO] WebView2 Runtime already installed. Skipping...
    ) else (
        echo [INFO] Downloading WebView2 Runtime Bootstrapper...
        powershell -Command "& {[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri '%WEBVIEW2_URL%' -OutFile '%TOOLS_DIR%\MicrosoftEdgeWebview2Setup.exe'}" 2>nul
        
        if exist "%TOOLS_DIR%\MicrosoftEdgeWebview2Setup.exe" (
            echo [INFO] Installing WebView2 Runtime...
            "%TOOLS_DIR%\MicrosoftEdgeWebview2Setup.exe" /silent /install
            timeout /t 10 /nobreak >nul
            echo [OK] WebView2 Runtime installed successfully.
        ) else (
            echo [WARNING] Could not download WebView2. Please install manually from:
            echo           https://developer.microsoft.com/en-us/microsoft-edge/webview2/
        )
    )
)
echo.

:: ----------------------------------------
:: STEP 4: Check/Install MySQL
:: ----------------------------------------
echo [STEP 4/6] Checking MySQL installation...

:: Check if MySQL is installed
set "MYSQL_FOUND=0"
set "MYSQL_PATH="

:: Check common MySQL installation paths
if exist "C:\Program Files\MySQL\MySQL Server 8.0\bin\mysql.exe" (
    set "MYSQL_FOUND=1"
    set "MYSQL_PATH=C:\Program Files\MySQL\MySQL Server 8.0"
)
if exist "C:\Program Files\MySQL\MySQL Server 8.4\bin\mysql.exe" (
    set "MYSQL_FOUND=1"
    set "MYSQL_PATH=C:\Program Files\MySQL\MySQL Server 8.4"
)
if exist "C:\Program Files\MySQL\MySQL Server 9.0\bin\mysql.exe" (
    set "MYSQL_FOUND=1"
    set "MYSQL_PATH=C:\Program Files\MySQL\MySQL Server 9.0"
)

:: Also check via registry
for /f "tokens=2*" %%a in ('reg query "HKLM\SOFTWARE\MySQL AB" 2^>nul ^| findstr /i "MySQL Server"') do (
    set "MYSQL_FOUND=1"
)

if "%MYSQL_FOUND%"=="1" (
    echo [INFO] MySQL Server found at: %MYSQL_PATH%
    echo [INFO] Skipping MySQL installation...
) else (
    echo [INFO] MySQL Server not found.
    echo.
    echo ========================================
    echo   MySQL Installation Options
    echo ========================================
    echo.
    echo Option 1: Download MySQL Installer (Recommended)
    echo Option 2: Skip MySQL installation
    echo.
    set /p MYSQL_CHOICE="Enter choice (1 or 2): "
    
    if "!MYSQL_CHOICE!"=="1" (
        echo.
        echo [INFO] Downloading MySQL Installer...
        powershell -Command "& {[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri '%MYSQL_INSTALLER_URL%' -OutFile '%TOOLS_DIR%\mysql-installer.msi'}" 2>nul
        
        if exist "%TOOLS_DIR%\mysql-installer.msi" (
            echo [INFO] Starting MySQL Installer...
            echo.
            echo ========================================
            echo   IMPORTANT: MySQL Installation Guide
            echo ========================================
            echo.
            echo 1. Choose "Custom" installation type
            echo 2. Select these components:
            echo    - MySQL Server 8.0 (or latest)
            echo    - MySQL Shell (optional)
            echo    - Connector/C++ (REQUIRED for this app)
            echo 3. Set root password (remember it!)
            echo 4. Configure MySQL as Windows Service
            echo.
            echo Press any key to start the installer...
            pause >nul
            
            msiexec /i "%TOOLS_DIR%\mysql-installer.msi"
            
            echo.
            echo [INFO] After MySQL installation completes, press any key to continue...
            pause >nul
        ) else (
            echo [ERROR] Could not download MySQL Installer.
            echo [INFO] Please download manually from: https://dev.mysql.com/downloads/installer/
        )
    ) else (
        echo [INFO] Skipping MySQL installation...
        echo [WARNING] You will need to install MySQL Server manually.
    )
)
echo.

:: ----------------------------------------
:: STEP 5: Configure MySQL Path
:: ----------------------------------------
echo [STEP 5/6] Configuring MySQL environment...

:: Find MySQL path
set "MYSQL_BIN="
if exist "C:\Program Files\MySQL\MySQL Server 8.0\bin" set "MYSQL_BIN=C:\Program Files\MySQL\MySQL Server 8.0\bin"
if exist "C:\Program Files\MySQL\MySQL Server 8.4\bin" set "MYSQL_BIN=C:\Program Files\MySQL\MySQL Server 8.4\bin"
if exist "C:\Program Files\MySQL\MySQL Server 9.0\bin" set "MYSQL_BIN=C:\Program Files\MySQL\MySQL Server 9.0\bin"

if defined MYSQL_BIN (
    :: Check if already in PATH
    echo %PATH% | findstr /i "MySQL" >nul
    if %errorlevel% neq 0 (
        echo [INFO] Adding MySQL to system PATH...
        setx PATH "%PATH%;%MYSQL_BIN%" /M >nul 2>&1
        echo [OK] MySQL added to PATH.
    ) else (
        echo [INFO] MySQL already in PATH.
    )
    
    :: Copy libmysql.dll to project directory
    if exist "%MYSQL_BIN%\..\lib\libmysql.dll" (
        echo [INFO] Copying libmysql.dll to project directory...
        copy /Y "%MYSQL_BIN%\..\lib\libmysql.dll" "%INSTALL_DIR%" >nul 2>&1
        echo [OK] libmysql.dll copied.
    )
) else (
    echo [WARNING] MySQL bin directory not found. Please add manually to PATH.
)
echo.

:: ----------------------------------------
:: STEP 6: Setup Database
:: ----------------------------------------
echo [STEP 6/6] Database Setup...
echo.

if defined MYSQL_BIN (
    echo ========================================
    echo   Database Configuration
    echo ========================================
    echo.
    echo Do you want to create the database now?
    echo (You will need to enter your MySQL root password)
    echo.
    set /p DB_SETUP="Create database? (Y/N): "
    
    if /i "!DB_SETUP!"=="Y" (
        echo.
        set /p MYSQL_ROOT_PASS="Enter MySQL root password: "
        
        echo [INFO] Creating database from schema...
        
        :: Check which schema file exists
        if exist "%INSTALL_DIR%sql\schema_v3.sql" (
            set "SCHEMA_FILE=%INSTALL_DIR%sql\schema_v3.sql"
        ) else if exist "%INSTALL_DIR%sql\schema_v2.sql" (
            set "SCHEMA_FILE=%INSTALL_DIR%sql\schema_v2.sql"
        ) else (
            echo [ERROR] No schema file found in sql\ directory!
            goto :skip_db
        )
        
        echo [INFO] Using schema: !SCHEMA_FILE!
        
        "%MYSQL_BIN%\mysql.exe" -u root -p!MYSQL_ROOT_PASS! < "!SCHEMA_FILE!" 2>nul
        
        if %errorlevel% equ 0 (
            echo [OK] Database created successfully!
            echo.
            echo ========================================
            echo   Default Login Credentials
            echo ========================================
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
            echo [ERROR] Failed to create database. Please check:
            echo         1. MySQL service is running
            echo         2. Password is correct
            echo         3. Run the SQL script manually in MySQL Workbench
        )
        
        :skip_db
    ) else (
        echo [INFO] Skipping database setup.
        echo [INFO] Run the SQL schema manually when ready:
        echo        mysql -u root -p ^< sql\schema_v2.sql
    )
) else (
    echo [INFO] MySQL not found. Please setup database manually after installing MySQL.
)

echo.
echo ========================================
echo   Verifying Installation
echo ========================================
echo.

:: Verify all components
set "ALL_OK=1"

:: Check VC++ Redistributable
reg query "HKLM\SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] Visual C++ Redistributable
) else (
    echo [MISSING] Visual C++ Redistributable
    set "ALL_OK=0"
)

:: Check WebView2
reg query "HKLM\SOFTWARE\WOW6432Node\Microsoft\EdgeUpdate\Clients\{F3017226-FE2A-4295-8BDF-00C3A9A7E4C5}" >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] Microsoft Edge WebView2 Runtime
) else (
    reg query "HKLM\SOFTWARE\Microsoft\EdgeUpdate\Clients\{F3017226-FE2A-4295-8BDF-00C3A9A7E4C5}" >nul 2>&1
    if %errorlevel% equ 0 (
        echo [OK] Microsoft Edge WebView2 Runtime
    ) else (
        echo [MISSING] Microsoft Edge WebView2 Runtime
        set "ALL_OK=0"
    )
)

:: Check MySQL
if exist "C:\Program Files\MySQL\MySQL Server 8.0\bin\mysql.exe" (
    echo [OK] MySQL Server 8.0
) else if exist "C:\Program Files\MySQL\MySQL Server 8.4\bin\mysql.exe" (
    echo [OK] MySQL Server 8.4
) else if exist "C:\Program Files\MySQL\MySQL Server 9.0\bin\mysql.exe" (
    echo [OK] MySQL Server 9.0
) else (
    echo [MISSING] MySQL Server
    set "ALL_OK=0"
)

:: Check libmysql.dll
if exist "%INSTALL_DIR%libmysql.dll" (
    echo [OK] libmysql.dll in project directory
) else (
    echo [MISSING] libmysql.dll - Copy from MySQL\lib folder
    set "ALL_OK=0"
)

:: Check webview header
if exist "%INSTALL_DIR%lib\webview.h" (
    echo [OK] WebView library header
) else (
    echo [MISSING] lib\webview.h
    set "ALL_OK=0"
)

echo.

if "%ALL_OK%"=="1" (
    echo ========================================
    echo   Setup Complete!
    echo ========================================
    echo.
    echo All dependencies are installed correctly.
    echo.
    echo Next steps:
    echo 1. Open the solution in Visual Studio 2022
    echo 2. Build the project (Ctrl+Shift+B)
    echo 3. Run the application (F5)
    echo.
) else (
    echo ========================================
    echo   Setup Incomplete
    echo ========================================
    echo.
    echo Some dependencies are missing. Please install them manually.
    echo.
)

:: Cleanup
echo.
set /p CLEANUP="Delete downloaded installers? (Y/N): "
if /i "%CLEANUP%"=="Y" (
    if exist "%TOOLS_DIR%" rd /s /q "%TOOLS_DIR%"
    echo [OK] Cleaned up temporary files.
)

echo.
echo Press any key to exit...
pause >nul

endlocal
exit /b 0
