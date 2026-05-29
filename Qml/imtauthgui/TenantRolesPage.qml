// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantRolesPage
 *
 * Roles tab — list / create / edit / delete tenant roles via the abstract apiClient.
 * Thin wrapper around `TenantSimpleCollectionPage` that wires up the role-specific
 * apiClient bindings and i18n strings.
 */
TenantSimpleCollectionPage {
	id: rolesPage

	entityName: qsTr("Role")
	entityNamePlural: qsTr("Roles")
	descriptionText: qsTr("Manage tenant roles and assign permissions to team members.")

	documentManager: apiClient ? apiClient.roleDocumentManager : null
	objectTypeId: apiClient ? apiClient.roleObjectTypeId : ""
	dataProvider: apiClient ? apiClient.roleListDataProvider : null

	function removeItem(id) {
		if (rolesPage.apiClient)
			rolesPage.apiClient.removeRole(id)
	}

	onApiClientChanged: {
		if (rolesPage.apiClient)
			rolesPage.apiClient.fetchPermissions()
	}

	Connections {
		target: rolesPage.apiClient
		function onRoleRemoved(roleId) {
			rolesPage.refresh()
		}
	}
}
