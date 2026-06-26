// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtauthPermissionsSdl 1.0
import imtauthgui 1.0

/**
 * GqlBasedPermissionsProvider
 *
 * SDL/GQL implementation of PermissionsProvider.
 * Sends GetProductPermissions with optional tenantId and stores both
 * product-wide and tenant-scoped caches.
 */
PermissionsProvider {
    id: gqlPermissionsProvider

    property GetProductPermissionsInput __requestInput: GetProductPermissionsInput {}
    property string __pendingTenantId: ""
    property string __pendingProductId: ""
    property string __allPermissionsProductId: ""
    property string __tenantPermissionsProductId: ""

    property GqlSdlRequestSender __requestSender: GqlSdlRequestSender {
        gqlCommandId: ImtauthPermissionsSdlCommandIds.s_getProductPermissions

        sdlObjectComp: Component {
            GetProductPermissionsPayload {
                onFinished: {
                    gqlPermissionsProvider.loading = false
                    if (m_errorMessage && m_errorMessage !== "") {
                        gqlPermissionsProvider.lastError = m_errorMessage
                        gqlPermissionsProvider.requestFailed(m_errorMessage, gqlPermissionsProvider.__pendingTenantId)
                    } else {
                        gqlPermissionsProvider.__storePermissions(m_groups, gqlPermissionsProvider.__pendingTenantId)
                    }
                }
            }
        }
    }

    function requestPermissions(tenantId, forceReload) {
        if (gqlPermissionsProvider.productId === "") {
            var message = "Unable to request permissions. Product-ID is empty"
            gqlPermissionsProvider.lastError = message
            gqlPermissionsProvider.requestFailed(message, tenantId || "")
            return
        }

        var requestedTenantId = tenantId || ""
        var shouldForceReload = forceReload === true

        // Reuse in-memory cache for identical tenant/product pair.
        if (!shouldForceReload
                && requestedTenantId !== ""
                && gqlPermissionsProvider.tenantPermissionsTenantId === requestedTenantId
            && gqlPermissionsProvider.__tenantPermissionsProductId === gqlPermissionsProvider.productId
                && gqlPermissionsProvider.tenantPermissions
                && gqlPermissionsProvider.tenantPermissions.length > 0) {
            gqlPermissionsProvider.permissions = gqlPermissionsProvider.tenantPermissions
            gqlPermissionsProvider.permissionsReceived(gqlPermissionsProvider.tenantPermissions, requestedTenantId)
            gqlPermissionsProvider.tenantPermissionsReceived(requestedTenantId)
            return
        }

        if (!shouldForceReload
            && requestedTenantId === ""
            && gqlPermissionsProvider.__allPermissionsProductId === gqlPermissionsProvider.productId
                && gqlPermissionsProvider.allPermissions
                && gqlPermissionsProvider.allPermissions.length > 0) {
            gqlPermissionsProvider.permissions = gqlPermissionsProvider.allPermissions
            gqlPermissionsProvider.permissionsReceived(gqlPermissionsProvider.allPermissions, "")
            gqlPermissionsProvider.allPermissionsReceived()
            return
        }

        // Ignore duplicate in-flight request for the same tenant/product pair.
        if (gqlPermissionsProvider.loading
                && gqlPermissionsProvider.__pendingTenantId === requestedTenantId
                && gqlPermissionsProvider.__pendingProductId === gqlPermissionsProvider.productId) {
            return
        }

        gqlPermissionsProvider.loading = true
        gqlPermissionsProvider.lastError = ""
        gqlPermissionsProvider.__pendingTenantId = requestedTenantId
        gqlPermissionsProvider.__pendingProductId = gqlPermissionsProvider.productId

        gqlPermissionsProvider.__requestInput.m_productId = gqlPermissionsProvider.productId
        gqlPermissionsProvider.__requestInput.m_tenantId = gqlPermissionsProvider.__pendingTenantId
        gqlPermissionsProvider.requestStarted(gqlPermissionsProvider.__pendingTenantId)
        gqlPermissionsProvider.__requestSender.send(gqlPermissionsProvider.__requestInput)
    }

    function __parseGroups(groupsList) {
        var result = []
        if (!groupsList)
            return result
        for (var gi = 0; gi < groupsList.count; ++gi) {
            var group = groupsList.get(gi).item
            if (!group)
                continue

            var groupObj = {
                "groupId": group.m_groupId || "",
                "groupName": group.m_groupName || "",
                "entries": []
            }

            var entries = group.m_entries
            if (entries) {
                for (var ei = 0; ei < entries.count; ++ei) {
                    var entry = entries.get(ei).item
                    if (!entry)
                        continue
                    groupObj.entries.push({
                        "permissionId": entry.m_permissionId || "",
                        "displayName": entry.m_displayName || "",
                        "description": entry.m_description || ""
                    })
                }
            }
            result.push(groupObj)
        }
        return result
    }

    function __storePermissions(groupsList, tenantId) {
        var parsedPermissions = gqlPermissionsProvider.__parseGroups(groupsList)
        gqlPermissionsProvider.permissions = parsedPermissions
        gqlPermissionsProvider.permissionsReceived(parsedPermissions, tenantId)

        if (tenantId && tenantId !== "") {
            gqlPermissionsProvider.tenantPermissionsTenantId = tenantId
            gqlPermissionsProvider.tenantPermissions = parsedPermissions
            gqlPermissionsProvider.__tenantPermissionsProductId = gqlPermissionsProvider.productId
            gqlPermissionsProvider.tenantPermissionsReceived(tenantId)
        } else {
            gqlPermissionsProvider.allPermissions = parsedPermissions
            gqlPermissionsProvider.__allPermissionsProductId = gqlPermissionsProvider.productId
            gqlPermissionsProvider.allPermissionsReceived()
        }
    }
}
