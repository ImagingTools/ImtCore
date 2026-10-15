import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0

/**
 * PermissionsProvider
 *
 * Abstract contract for loading product permissions.
 * Concrete implementations (e.g. GqlBasedPermissionsProvider) provide the actual transport.
 *
 * This file MUST NOT import imtguigql or any GQL module.
 */
QtObject {
    id: permissionsProvider

    property string productId: ""
    // Part of the product the provider works in; travels with every request it sends.
    property string permissionPath: ""
    property bool loading: false
    property string lastError: ""

    // Latest request result regardless of request scope.
    property var permissions: []

    // Cached product-wide permissions (requestPermissions("")).
    property var allPermissions: []

    // Cached permissions available to the currently authenticated user
    // (superuser sees the full product tree). Filled by requestUserPermissions().
    property var userPermissions: []
    signal userPermissionsReceived()

    // Cached tenant-scoped permissions (requestPermissions(tenantId)).
    property var tenantPermissions: []
    property string tenantPermissionsTenantId: ""

    // Special organization-only permissions tree (independent of product features)
    property var organizationPermissions: []
    // Assigned permissions for the last requested member (populated when userId passed to requestOrganizationPermissions)
    property var memberOrganizationPermissions: []
    signal organizationPermissionsReceived()

    signal requestStarted(string tenantId)
    signal permissionsReceived(var permissions, string tenantId)
    signal allPermissionsReceived()
    signal tenantPermissionsReceived(string tenantId)
    signal requestFailed(string message, string tenantId)

    // tenantId == "" means product-wide permissions request.
    function requestPermissions(tenantId) {}

    function requestAllPermissions() {
        requestPermissions("")
    }

    // Request only the permissions available to the current user (su sees all).
    function requestUserPermissions() {}

    function requestOrganizationPermissions(tenantId, userId) {}

    function clearCache() {
        permissions = []
        allPermissions = []
        userPermissions = []
        tenantPermissions = []
        tenantPermissionsTenantId = ""
        organizationPermissions = []
        memberOrganizationPermissions = []
        lastError = ""
    }

    function getHeaders() {
        return {}
    }
}
