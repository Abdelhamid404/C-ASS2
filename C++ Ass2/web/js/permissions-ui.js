/* ========================================
   Permission-Based UI Control
   ========================================
   This file handles showing/hiding UI elements
   based on user permissions
   ======================================== */

/**
 * PermissionUI Class
 * Manages UI elements visibility based on permissions
 */
class PermissionUI {
    constructor(userPermissions) {
        this.permissions = userPermissions || [];
    }

    /**
     * Apply permissions to the page
     * - Hides elements with data-permission if user doesn't have the permission
     * - Disables inputs with data-edit-permission if user doesn't have edit rights
     */
    applyPermissions() {
        // Hide elements that require specific permissions
        document.querySelectorAll('[data-permission]').forEach(el => {
            const required = el.dataset.permission;
            if (!this.hasPermission(required)) {
                el.style.display = 'none';
                el.classList.add('permission-hidden');
            }
        });
        
        // Disable edit fields if no edit permission
        document.querySelectorAll('[data-edit-permission]').forEach(el => {
            const required = el.dataset.editPermission;
            if (!this.hasPermission(required)) {
                el.disabled = true;
                el.readOnly = true;
                el.classList.add('permission-readonly');
            }
        });
    }

    /**
     * Check if user has a specific permission
     */
    hasPermission(permission) {
        return this.permissions.includes(permission);
    }

    /**
     * Show element if user has permission
     */
    showIfAllowed(elementId, permission) {
        const el = document.getElementById(elementId);
        if (el && this.hasPermission(permission)) {
            el.style.display = '';
            el.classList.remove('permission-hidden');
        } else if (el) {
            el.style.display = 'none';
            el.classList.add('permission-hidden');
        }
    }

    /**
     * Enable element if user has permission
     */
    enableIfAllowed(elementId, permission) {
        const el = document.getElementById(elementId);
        if (el && this.hasPermission(permission)) {
            el.disabled = false;
            el.readOnly = false;
            el.classList.remove('permission-readonly');
        } else if (el) {
            el.disabled = true;
            el.readOnly = true;
            el.classList.add('permission-readonly');
        }
    }

    /**
     * Dynamic button generator based on permissions
     * Returns HTML string for a button if user has permission
     */
    renderButton(options) {
        const {
            permission,
            text,
            className = 'btn btn-primary',
            onclick = '',
            icon = ''
        } = options;

        if (!this.hasPermission(permission)) {
            return '';
        }

        const iconHtml = icon ? `<i class="${icon}"></i> ` : '';
        return `<button class="${className}" onclick="${onclick}">${iconHtml}${text}</button>`;
    }

    /**
     * Dynamic action buttons for table rows
     */
    renderActionButtons(permissions) {
        let buttons = '';
        
        if (permissions.view && this.hasPermission(permissions.view.permission)) {
            buttons += `<button class="btn-action btn-view" onclick="${permissions.view.onclick}">👁️ View</button> `;
        }
        
        if (permissions.edit && this.hasPermission(permissions.edit.permission)) {
            buttons += `<button class="btn-action btn-edit" onclick="${permissions.edit.onclick}">✏️ Edit</button> `;
        }
        
        if (permissions.delete && this.hasPermission(permissions.delete.permission)) {
            buttons += `<button class="btn-action btn-delete" onclick="${permissions.delete.onclick}">🗑️ Delete</button>`;
        }
        
        return buttons || '<span class="text-muted">No actions</span>';
    }
}

// Global instance
let permissionUI = null;

/**
 * Initialize permission system
 * Call this on page load
 */
function initPermissionSystem() {
    const permissions = getPermissions(); // from app.js
    permissionUI = new PermissionUI(permissions);
    permissionUI.applyPermissions();
    return permissionUI;
}

// Auto-initialize if DOM is ready
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', initPermissionSystem);
} else {
    initPermissionSystem();
}
