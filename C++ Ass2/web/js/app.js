/* ========================================
   Student Information System - Main JavaScript
   ========================================
   This file contains shared JavaScript functions
   used across all pages of the application.
   ======================================== */

// ========================================
// GLOBAL VARIABLES
// ========================================
const USER_ROLE = sessionStorage.getItem('userRole');
const USER_ID = sessionStorage.getItem('userId');
const USER_NAME = sessionStorage.getItem('userName');
const USER_ROLE_NAME = sessionStorage.getItem('roleName');

// ========================================
// AUTHENTICATION CHECK
// ========================================
/**
 * checkAuth - Verifies user is logged in
 * 
 * Redirects to login page if not authenticated.
 * Call this at the start of each protected page.
 */
function checkAuth() {
    if (!USER_ROLE) {
        window.location.href = 'index.html';
        return false;
    }
    return true;
}

// ========================================
// PERMISSIONS & ROLE HELPERS
// ========================================
function getPermissions() {
    const raw = sessionStorage.getItem('permissions');
    if (!raw) return [];
    try {
        const parsed = JSON.parse(raw);
        return Array.isArray(parsed) ? parsed : [];
    } catch (e) {
        return [];
    }
}

function hasPermission(name) {
    return getPermissions().includes(name);
}

function isRole(roleId) {
    return USER_ROLE === roleId;
}

// ========================================
// LAYOUT HELPERS
// ========================================
function buildSidebarMenu(activeKey = '') {
    const menu = document.getElementById('sidebarMenu');
    if (!menu) return;

    const items = [
        { key: 'dashboard', label: 'Dashboard', href: 'dashboard.html', icon: 'bi-speedometer2', show: true },
        { key: 'students', label: 'Students', href: 'students.html', icon: 'bi-people', show: hasPermission('student.view') },
        { key: 'professors', label: 'Professors', href: 'professors.html', icon: 'bi-person-badge', show: hasPermission('professor.view') },
        { key: 'courses', label: 'Courses', href: 'courses.html', icon: 'bi-book', show: hasPermission('course.view') },
        { key: 'registrations', label: 'Registrations', href: 'registrations.html', icon: 'bi-clipboard-check', show: hasPermission('registration.view') },
        { key: 'grades', label: 'Grades', href: 'grades.html', icon: 'bi-bar-chart', show: hasPermission('grade.view.all') || hasPermission('grade.view.assigned') || hasPermission('grade.enter') },
        { key: 'attendance', label: 'Attendance', href: 'attendance.html', icon: 'bi-calendar-check', show: hasPermission('attendance.view.all') || hasPermission('attendance.record') },
        { key: 'fees', label: 'Student Fees', href: 'fees.html', icon: 'bi-cash-stack', show: hasPermission('fee.view') },
        { key: 'my-grades', label: 'My Grades', href: 'my-grades.html', icon: 'bi-mortarboard', show: isRole('ROLE_STUDENT') },
        { key: 'colleges', label: 'Colleges & Depts', href: 'colleges.html', icon: 'bi-building', show: hasPermission('student.view') || hasPermission('course.view') },
        { key: 'halls', label: 'Halls & Labs', href: 'halls.html', icon: 'bi-door-open', show: hasPermission('student.view') || hasPermission('course.view') },
        { key: 'users', label: 'Users', href: 'users.html', icon: 'bi-people-fill', show: hasPermission('user.manage') },
        { key: 'permissions', label: 'Permissions', href: 'permissions.html', icon: 'bi-shield-lock', show: hasPermission('user.manage') }
    ];

    menu.innerHTML = items
        .filter(item => item.show)
        .map(item => {
            const isActive = item.key === activeKey ? ' active bg-primary text-white' : ' text-white';
            return `
                <li class="nav-item">
                    <a class="nav-link${isActive}" href="${item.href}">
                        <i class="bi ${item.icon} me-2"></i> ${item.label}
                    </a>
                </li>
            `;
        })
        .join('');
}

function initLayout(activeKey = '') {
    if (!checkAuth()) return;

    const nameEl = document.getElementById('userNameDisplay');
    if (nameEl) {
        nameEl.textContent = USER_NAME || 'User';
    }

    const roleEl = document.getElementById('userRoleBadge');
    if (roleEl) {
        roleEl.textContent = USER_ROLE_NAME || USER_ROLE || 'User';
        const badgeColors = {
            ROLE_SUPERADMIN: 'bg-danger',
            ROLE_STUDENT_AFFAIRS: 'bg-warning text-dark',
            ROLE_PROFESSOR: 'bg-info',
            ROLE_STUDENT: 'bg-success'
        };
        roleEl.className = 'badge ' + (badgeColors[USER_ROLE] || 'bg-primary');
    }

    buildSidebarMenu(activeKey);
    
    // Apply permission-based UI controls
    if (typeof initPermissionSystem === 'function') {
        initPermissionSystem();
    }
}

// ========================================
// LOGOUT FUNCTION
// ========================================
/**
 * logout - Clears session and redirects to login
 */
function logout() {
    sessionStorage.clear();
    window.location.href = 'index.html';
}

// ========================================
// TOAST NOTIFICATIONS
// ========================================
/**
 * showToast - Displays a toast notification
 * 
 * @param {string} message - The message to display
 * @param {string} type - 'success', 'danger', 'warning', 'info'
 * @param {number} duration - How long to show (ms), default 3000
 */
function showToast(message, type = 'success', duration = 3000) {
    // Get or create toast container
    let container = document.querySelector('.toast-container');
    if (!container) {
        container = document.createElement('div');
        container.className = 'toast-container position-fixed top-0 end-0 p-3';
        document.body.appendChild(container);
    }

    // Determine icon based on type
    const icons = {
        success: 'bi-check-circle-fill',
        danger: 'bi-exclamation-triangle-fill',
        warning: 'bi-exclamation-circle-fill',
        info: 'bi-info-circle-fill'
    };

    // Create toast element
    const toast = document.createElement('div');
    toast.className = `toast align-items-center text-white bg-${type} border-0`;
    toast.setAttribute('role', 'alert');
    toast.setAttribute('aria-live', 'assertive');
    toast.setAttribute('aria-atomic', 'true');
    
    toast.innerHTML = `
        <div class="d-flex">
            <div class="toast-body">
                <i class="bi ${icons[type] || icons.info} me-2"></i>
                ${message}
            </div>
            <button type="button" class="btn-close btn-close-white me-2 m-auto" 
                    data-bs-dismiss="toast" aria-label="Close"></button>
        </div>
    `;

    container.appendChild(toast);

    // Initialize and show toast
    const bsToast = new bootstrap.Toast(toast, {
        autohide: true,
        delay: duration
    });
    bsToast.show();

    // Remove from DOM after hidden
    toast.addEventListener('hidden.bs.toast', () => {
        toast.remove();
    });
}

// ========================================
// LOADING OVERLAY
// ========================================
/**
 * showLoading - Shows a loading overlay
 * 
 * @param {string} message - Optional loading message
 */
function showLoading(message = 'Loading...') {
    // Remove existing overlay if any
    hideLoading();

    const overlay = document.createElement('div');
    overlay.id = 'loadingOverlay';
    overlay.className = 'spinner-overlay';
    overlay.innerHTML = `
        <div class="text-center">
            <div class="spinner-border text-primary" style="width: 3rem; height: 3rem;" role="status">
                <span class="visually-hidden">Loading...</span>
            </div>
            <p class="mt-3 text-muted">${message}</p>
        </div>
    `;

    document.body.appendChild(overlay);
}

/**
 * hideLoading - Hides the loading overlay
 */
function hideLoading() {
    const overlay = document.getElementById('loadingOverlay');
    if (overlay) {
        overlay.remove();
    }
}

// ========================================
// DATE FORMATTING
// ========================================
/**
 * formatDate - Formats a date string for display
 * 
 * @param {string} dateString - Date in YYYY-MM-DD format
 * @param {string} format - 'short', 'long', 'full'
 * @returns {string} Formatted date
 */
function formatDate(dateString, format = 'long') {
    if (!dateString) return '-';

    const date = new Date(dateString);
    
    if (isNaN(date.getTime())) return dateString;

    const options = {
        short: { year: 'numeric', month: 'short', day: 'numeric' },
        long: { year: 'numeric', month: 'long', day: 'numeric' },
        full: { weekday: 'long', year: 'numeric', month: 'long', day: 'numeric' }
    };

    return date.toLocaleDateString('en-US', options[format] || options.long);
}

/**
 * formatTime - Formats a time string for display
 * 
 * @param {string} timeString - Time in HH:MM:SS format
 * @returns {string} Formatted time (12-hour format)
 */
function formatTime(timeString) {
    if (!timeString) return '-';

    const [hours, minutes] = timeString.split(':');
    const hour = parseInt(hours);
    const ampm = hour >= 12 ? 'PM' : 'AM';
    const displayHour = hour % 12 || 12;

    return `${displayHour}:${minutes} ${ampm}`;
}

// ========================================
// GRADE HELPERS
// ========================================
/**
 * getGradeClass - Returns CSS class for grade evaluation
 * 
 * @param {string} evaluation - Excellent, Very Good, Good, Pass, Fail
 * @returns {string} CSS class name
 */
function getGradeClass(evaluation) {
    const classes = {
        'Excellent': 'grade-excellent',
        'Very Good': 'grade-very-good',
        'Good': 'grade-good',
        'Pass': 'grade-pass',
        'Fail': 'grade-fail'
    };
    return classes[evaluation] || '';
}

/**
 * getGradeBadgeClass - Returns Bootstrap badge class for grade
 * 
 * @param {string} evaluation - Excellent, Very Good, Good, Pass, Fail
 * @returns {string} Bootstrap badge class
 */
function getGradeBadgeClass(evaluation) {
    const classes = {
        'Excellent': 'bg-success',
        'Very Good': 'bg-info',
        'Good': 'bg-primary',
        'Pass': 'bg-warning text-dark',
        'Fail': 'bg-danger'
    };
    return classes[evaluation] || 'bg-secondary';
}

// ========================================
// TABLE HELPERS
// ========================================
/**
 * showTableLoading - Shows loading state in a table
 * 
 * @param {string} tbodyId - ID of the tbody element
 * @param {number} colSpan - Number of columns to span
 */
function showTableLoading(tbodyId, colSpan = 5) {
    const tbody = document.getElementById(tbodyId);
    if (tbody) {
        tbody.innerHTML = `
            <tr>
                <td colspan="${colSpan}" class="text-center py-4">
                    <div class="spinner-border text-primary" role="status">
                        <span class="visually-hidden">Loading...</span>
                    </div>
                    <p class="mt-2 mb-0 text-muted">Loading data...</p>
                </td>
            </tr>
        `;
    }
}

/**
 * showTableEmpty - Shows empty state in a table
 * 
 * @param {string} tbodyId - ID of the tbody element
 * @param {number} colSpan - Number of columns to span
 * @param {string} message - Empty state message
 */
function showTableEmpty(tbodyId, colSpan = 5, message = 'No data found') {
    const tbody = document.getElementById(tbodyId);
    if (tbody) {
        tbody.innerHTML = `
            <tr>
                <td colspan="${colSpan}" class="text-center py-4 text-muted">
                    <i class="bi bi-inbox display-4 d-block mb-2"></i>
                    ${message}
                </td>
            </tr>
        `;
    }
}

// ========================================
// FORM VALIDATION
// ========================================
/**
 * validateForm - Validates a form and shows errors
 * 
 * @param {HTMLFormElement} form - The form to validate
 * @returns {boolean} True if valid
 */
function validateForm(form) {
    form.classList.add('was-validated');
    return form.checkValidity();
}

/**
 * clearFormValidation - Clears form validation state
 * 
 * @param {HTMLFormElement} form - The form to clear
 */
function clearFormValidation(form) {
    form.classList.remove('was-validated');
    form.querySelectorAll('.is-invalid').forEach(el => {
        el.classList.remove('is-invalid');
    });
}

// ========================================
// CONFIRMATION DIALOG
// ========================================
/**
 * confirmAction - Shows a confirmation dialog
 * 
 * @param {string} message - Confirmation message
 * @param {Function} onConfirm - Function to call on confirm
 * @param {string} type - 'danger', 'warning', 'primary'
 */
function confirmAction(message, onConfirm, type = 'danger') {
    // Create modal HTML
    const modalId = 'confirmModal' + Date.now();
    const modalHtml = `
        <div class="modal fade" id="${modalId}" tabindex="-1">
            <div class="modal-dialog">
                <div class="modal-content">
                    <div class="modal-header bg-${type} text-white">
                        <h5 class="modal-title">
                            <i class="bi bi-exclamation-triangle me-2"></i>
                            Confirm Action
                        </h5>
                        <button type="button" class="btn-close btn-close-white" data-bs-dismiss="modal"></button>
                    </div>
                    <div class="modal-body">
                        <p class="mb-0">${message}</p>
                    </div>
                    <div class="modal-footer">
                        <button type="button" class="btn btn-secondary" data-bs-dismiss="modal">Cancel</button>
                        <button type="button" class="btn btn-${type}" id="${modalId}Confirm">Confirm</button>
                    </div>
                </div>
            </div>
        </div>
    `;

    // Add to DOM
    document.body.insertAdjacentHTML('beforeend', modalHtml);

    const modal = new bootstrap.Modal(document.getElementById(modalId));
    
    // Handle confirm button
    document.getElementById(`${modalId}Confirm`).addEventListener('click', () => {
        modal.hide();
        if (typeof onConfirm === 'function') {
            onConfirm();
        }
    });

    // Clean up after hidden
    document.getElementById(modalId).addEventListener('hidden.bs.modal', function() {
        this.remove();
    });

    modal.show();
}

// ========================================
// DATA EXPORT HELPERS
// ========================================
/**
 * exportToCSV - Exports data array to CSV file
 * 
 * @param {Array} data - Array of objects to export
 * @param {string} filename - Name of the file (without extension)
 */
function exportToCSV(data, filename = 'export') {
    if (!data || data.length === 0) {
        showToast('No data to export', 'warning');
        return;
    }

    // Get headers from first object
    const headers = Object.keys(data[0]);
    
    // Create CSV content
    const csvContent = [
        headers.join(','),
        ...data.map(row => 
            headers.map(header => {
                let value = row[header];
                // Escape commas and quotes
                if (typeof value === 'string') {
                    value = value.replace(/"/g, '""');
                    if (value.includes(',') || value.includes('"') || value.includes('\n')) {
                        value = `"${value}"`;
                    }
                }
                return value;
            }).join(',')
        )
    ].join('\n');

    // Create and download file
    const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8;' });
    const link = document.createElement('a');
    link.href = URL.createObjectURL(blob);
    link.download = `${filename}_${new Date().toISOString().split('T')[0]}.csv`;
    link.click();

    showToast('Data exported successfully', 'success');
}

// ========================================
// DEBOUNCE UTILITY
// ========================================
/**
 * debounce - Limits how often a function can be called
 * 
 * @param {Function} func - Function to debounce
 * @param {number} wait - Wait time in milliseconds
 * @returns {Function} Debounced function
 */
function debounce(func, wait = 300) {
    let timeout;
    return function executedFunction(...args) {
        const later = () => {
            clearTimeout(timeout);
            func(...args);
        };
        clearTimeout(timeout);
        timeout = setTimeout(later, wait);
    };
}

// ========================================
// URL PARAMETER HELPERS
// ========================================
/**
 * getUrlParam - Gets a URL parameter value
 * 
 * @param {string} param - Parameter name
 * @returns {string|null} Parameter value or null
 */
function getUrlParam(param) {
    const urlParams = new URLSearchParams(window.location.search);
    return urlParams.get(param);
}

// ========================================
// CONSOLE LOGGING (Development)
// ========================================
/**
 * log - Console log wrapper (can be disabled in production)
 */
const DEBUG = true;

function log(...args) {
    if (DEBUG) {
        console.log('[SIS]', ...args);
    }
}

function logError(...args) {
    console.error('[SIS ERROR]', ...args);
}

// ========================================
// SAFE NAVIGATION - Handles missing pages
// ========================================
/**
 * safeNavigate - Navigates to a page, handling missing files
 * 
 * @param {string} page - Page URL to navigate to
 */
function safeNavigate(page) {
    // List of available pages
    const availablePages = [
        'index.html',
        'dashboard.html',
        'students.html',
        'professors.html',
        'courses.html',
        'colleges.html',
        'halls.html',
        'registrations.html',
        'grades.html',
        'my-grades.html',
        'attendance.html',
        'permissions.html',
        'fees.html',
        'users.html'
    ];
    
    // Extract just the filename
    const pageName = page.split('/').pop().split('?')[0];
    
    if (availablePages.includes(pageName)) {
        window.location.href = page;
    } else {
        console.warn('[SIS] Page not found:', page);
        showToast('Page not available yet: ' + pageName, 'warning');
        // Stay on current page instead of navigating
    }
}

// ========================================
// GLOBAL ERROR HANDLER
// ========================================
window.addEventListener('error', function(e) {
    console.error('[SIS] Error:', e.message);
});

// Handle navigation errors (file not found)
window.addEventListener('unhandledrejection', function(e) {
    console.error('[SIS] Unhandled rejection:', e.reason);
});

// ========================================
// INITIALIZE ON PAGE LOAD
// ========================================
document.addEventListener('DOMContentLoaded', function() {
    // Set current date in header if element exists
    const dateDisplay = document.getElementById('dateDisplay');
    if (dateDisplay) {
        dateDisplay.textContent = formatDate(new Date().toISOString().split('T')[0], 'full');
    }

    // Highlight active menu item
    const currentPage = window.location.pathname.split('/').pop();
    document.querySelectorAll('#sidebar .nav-link').forEach(link => {
        if (link.getAttribute('href') === currentPage) {
            link.classList.add('active');
        }
    });

    // Intercept clicks on links to unavailable pages
    document.addEventListener('click', function(e) {
        const link = e.target.closest('a');
        if (link && link.href && link.href.includes('.html')) {
            const href = link.getAttribute('href');
            const availablePages = [
                'index.html',
                'dashboard.html',
                'students.html',
                'professors.html',
                'courses.html',
                'colleges.html',
                'halls.html',
                'registrations.html',
                'grades.html',
                'my-grades.html',
                'attendance.html',
                'permissions.html',
                'fees.html',
                'users.html'
            ];
            
            const pageName = href.split('/').pop().split('?')[0];
            
            if (!availablePages.includes(pageName) && !href.startsWith('http')) {
                e.preventDefault();
                showToast('This page is not available yet: ' + pageName, 'warning');
            }
        }
    });

    log('Application initialized');
});
