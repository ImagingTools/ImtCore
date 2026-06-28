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
 * Dumb SDL/GQL implementation of PermissionsProvider.
 * When requestPermissions(tenantId) is called it validates productId and sends
 * the GetProductPermissions query. No in-memory cache short-circuits, no
 * force/dedup logic. Response handling stores results and emits signals.
 */
PermissionsProvider {
    id: root

    property GetProductPermissionsInput __requestInput: GetProductPermissionsInput {}
    // Remember the tenant for the in-flight request (response payload does not echo it).
    // Used only to route result into correct scoped properties and emit the right signals.
    property string __pendingTenantId: ""

    property GqlSdlRequestSender __requestSender: GqlSdlRequestSender {
        gqlCommandId: ImtauthPermissionsSdlCommandIds.s_getProductPermissions

        sdlObjectComp: Component {
            GetProductPermissionsPayload {
                onFinished: {
                    root.loading = false
                    if (m_errorMessage && m_errorMessage !== "") {
                        root.lastError = m_errorMessage
                        root.requestFailed(m_errorMessage, root.__pendingTenantId)
                    } else {
                        root.__storePermissions(m_groups, root.__pendingTenantId)
                    }
                }
            }
        }
    }

    function requestPermissions(tenantId) {
        if (root.productId === "") {
            var message = "Unable to request permissions. Product-ID is empty"
            root.lastError = message
            root.requestFailed(message, tenantId || "")
            return
        }

        var requestedTenantId = tenantId || ""

        root.loading = true
        root.lastError = ""
        root.__pendingTenantId = requestedTenantId

        root.__requestInput.m_productId = root.productId
        root.__requestInput.m_tenantId = requestedTenantId
        root.requestStarted(requestedTenantId)
        root.__requestSender.send(root.__requestInput)
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
        var parsedPermissions = root.__parseGroups(groupsList)
        root.permissions = parsedPermissions
        root.permissionsReceived(parsedPermissions, tenantId)

        if (tenantId && tenantId !== "") {
            root.tenantPermissionsTenantId = tenantId
            root.tenantPermissions = parsedPermissions
            root.tenantPermissionsReceived(tenantId)
        } else {
            root.allPermissions = parsedPermissions
            root.allPermissionsReceived()
        }
    }
}
