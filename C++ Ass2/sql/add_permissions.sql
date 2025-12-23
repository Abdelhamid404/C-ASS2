-- ================================================================================
-- Add new permissions for password viewing and role management
-- Run this to update an existing database
-- ================================================================================

USE nctu_sis;

-- Add new permissions if they don't exist
INSERT IGNORE INTO permissions (id, name, description) VALUES
('PERM_PWD_VIEW_ALL', 'password.view.all', 'View all user passwords'),
('PERM_PWD_VIEW_STU', 'password.view.student', 'View student passwords only'),
('PERM_ROLE_MANAGE', 'role.manage', 'Create/edit/delete roles');

-- Grant new permissions to Super Admin
INSERT IGNORE INTO role_permissions (role_id, permission_id) VALUES
('ROLE_SUPERADMIN', 'PERM_PWD_VIEW_ALL'),
('ROLE_SUPERADMIN', 'PERM_ROLE_MANAGE');

-- Grant student password view to Student Affairs
INSERT IGNORE INTO role_permissions (role_id, permission_id) VALUES
('ROLE_STUDENT_AFFAIRS', 'PERM_PWD_VIEW_STU');

SELECT 'New permissions added successfully!' AS status;



