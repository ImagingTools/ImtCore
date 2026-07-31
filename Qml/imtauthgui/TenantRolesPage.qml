// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtdocgui 1.0
import imtauthgui 1.0

/**
 * TenantRolesPage
 *
 * Roles tab — list / create / edit / delete tenant roles via the abstract apiClient.
 * Thin wrapper around `SimpleCollectionPage` that wires up the role-specific
 * apiClient bindings and i18n strings.
 */
SimpleCollectionPage {
	id: rolesPage

	entityName: qsTr("Role")
	entityNamePlural: qsTr("Roles")
	descriptionText: qsTr("Manage tenant roles and assign permissions to team members.")

	documentManager: apiClient ? apiClient.roleDocumentManager : null
	objectTypeId: apiClient ? apiClient.roleObjectTypeId : ""
	dataProvider: apiClient ? apiClient.roleListDataProvider : null
	managePermissionIds: ["EditOrganizationRole", "ChangeOrganizationRole", "RemoveOrganizationRole", "AddOrganizationRole"]
	createPermissionIds: ["AddOrganizationRole", "EditOrganizationRole"]
	editPermissionIds: ["ChangeOrganizationRole", "EditOrganizationRole"]
	deletePermissionIds: ["RemoveOrganizationRole", "EditOrganizationRole"]
	property string __lastPermissionsTenantId: ""

	function removeItems(ids) {
		if (rolesPage.apiClient)
			rolesPage.apiClient.removeRoles((ids))
	}

	function __resolvedTenantId() {
		if (rolesPage.model && rolesPage.model.m_id)
			return rolesPage.model.m_id
		return ""
	}

	function __requestTenantPermissions(force) {
		if (!rolesPage.apiClient)
			return false

		var tenantId = rolesPage.__resolvedTenantId()
		if (tenantId === "")
			return false

		if (!force
				&& rolesPage.__lastPermissionsTenantId === tenantId)
			return true

		rolesPage.__lastPermissionsTenantId = tenantId
		if (rolesPage.apiClient.setRolePermissionsTenantId)
			rolesPage.apiClient.setRolePermissionsTenantId(tenantId)
		else
			rolesPage.apiClient.rolePermissionsTenantId = tenantId
		return true
	}

	function __scheduleTenantPermissionsRequest(force) {
		if (rolesPage.__requestTenantPermissions(force)) {
			permissionsRetryTimer.stop()
			return
		}
		permissionsRetryTimer.restart()
	}

	Timer {
		id: permissionsRetryTimer
		interval: 250
		repeat: true
		running: false
		onTriggered: {
			if (!rolesPage.visible)
				return
			if (rolesPage.__requestTenantPermissions(false))
				permissionsRetryTimer.stop()
		}
	}

	onApiClientChanged: {
		rolesPage.__lastPermissionsTenantId = ""
		rolesPage.__scheduleTenantPermissionsRequest(true)
	}

	onVisibleChanged: {
		if (visible)
			rolesPage.__scheduleTenantPermissionsRequest(false)
		else
			permissionsRetryTimer.stop()
	}

	Component.onCompleted: {
		rolesPage.__scheduleTenantPermissionsRequest(true)
	}

	Connections {
		target: rolesPage.apiClient
		function onRolesRemoved() {
			rolesPage.refresh()
		}
	}

	Connections {
		target: rolesPage.model
		function onM_idChanged() {
			rolesPage.__lastPermissionsTenantId = ""
			rolesPage.__scheduleTenantPermissionsRequest(true)
		}
	}
}
