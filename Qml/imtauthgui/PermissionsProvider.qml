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
    property bool loading: false
    property string lastError: ""

    // Latest request result regardless of request scope.
    property var permissions: []

    // Cached product-wide permissions (requestPermissions("")).
    property var allPermissions: []

    // Cached tenant-scoped permissions (requestPermissions(tenantId)).
    property var tenantPermissions: []
    property string tenantPermissionsTenantId: ""

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

    function clearCache() {
        permissions = []
        allPermissions = []
        tenantPermissions = []
        tenantPermissionsTenantId = ""
        lastError = ""
    }

    function getHeaders() {
        return {}
    }
}
