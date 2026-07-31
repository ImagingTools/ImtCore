// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtauthPermissionsSdl 1.0
import imtauthTenantMembershipsSdl 1.0
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
    property GetUserPermissionsInput __userRequestInput: GetUserPermissionsInput {}
    property GetOrganizationPermissionsInput __orgRequestInput: GetOrganizationPermissionsInput {}
    // Remember the tenant for the in-flight request (response payload does not echo it).
    // Used only to route result into correct scoped properties and emit the right signals.
    property string __pendingTenantId: ""
    property string __pendingOrgTenantId: ""

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

    property GqlSdlRequestSender __userRequestSender: GqlSdlRequestSender {
        gqlCommandId: ImtauthPermissionsSdlCommandIds.s_getUserPermissions

        sdlObjectComp: Component {
            GetProductPermissionsPayload {
                onFinished: {
                    root.loading = false
                    if (m_errorMessage && m_errorMessage !== "") {
                        root.lastError = m_errorMessage
                        root.requestFailed(m_errorMessage, "")
                    } else {
                        root.__storeUserPermissions(m_groups)
                    }
                }
            }
        }
    }

    function requestUserPermissions() {
        if (root.productId === "") {
            var message = "Unable to request permissions. Product-ID is empty"
            root.lastError = message
            root.requestFailed(message, "")
            return
        }

        root.loading = true
        root.lastError = ""
        root.__userRequestInput.m_productId = root.productId
        root.requestStarted("")
        root.__userRequestSender.send(root.__userRequestInput)
    }

    property GqlSdlRequestSender __orgRequestSender: GqlSdlRequestSender {
        gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_getOrganizationPermissions

        sdlObjectComp: Component {
            GetOrganizationPermissionsPayload {
                onFinished: {
                    root.loading = false
                    if (m_errorMessage && m_errorMessage !== "") {
                        root.lastError = m_errorMessage
                        root.requestFailed(m_errorMessage, root.__pendingOrgTenantId)
                    } else {
                        root.__storeOrganizationPermissions(m_groups, m_memberPermissions, root.__pendingOrgTenantId)
                    }
                }
            }
        }
    }

    function requestOrganizationPermissions(tenantId, userId) {
        root.loading = true
        root.lastError = ""
        root.__pendingOrgTenantId = tenantId || ""
        root.__orgRequestInput.m_tenantId = root.__pendingOrgTenantId
        root.__orgRequestInput.m_userId = userId || ""
        root.requestStarted(root.__pendingOrgTenantId)
        root.__orgRequestSender.send(root.__orgRequestInput)
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

    function __storeUserPermissions(groupsList) {
        var parsedPermissions = root.__parseGroups(groupsList)
        root.permissions = parsedPermissions
        root.userPermissions = parsedPermissions
        root.userPermissionsReceived()
    }

    function __storeOrganizationPermissions(groupsList, memberPermsList, tenantId) {
        var parsed = root.__parseGroups(groupsList)
        root.organizationPermissions = parsed

        // m_memberPermissions is a plain JS array ([ID] maps to var [])
        root.memberOrganizationPermissions = Array.isArray(memberPermsList)
            ? memberPermsList.filter(function(s) { return s && s.length > 0 })
            : []

        root.organizationPermissionsReceived()
    }
}
