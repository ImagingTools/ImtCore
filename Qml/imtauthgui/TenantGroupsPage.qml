// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantGroupsPage
 *
 * Groups tab — list / create / edit / delete tenant groups via the abstract apiClient.
 * Thin wrapper around `TenantSimpleCollectionPage` that wires up the group-specific
 * apiClient bindings and i18n strings.
 */
TenantSimpleCollectionPage {
	id: groupsPage

	entityName: qsTr("Group")
	entityNamePlural: qsTr("Groups")
	descriptionText: qsTr("Organize members into groups for easier permission management.")

	documentManager: apiClient ? apiClient.groupDocumentManager : null
	objectTypeId: apiClient ? apiClient.groupObjectTypeId : ""
	dataProvider: apiClient ? apiClient.groupListDataProvider : null
	managePermissionIds: ["EditOrganizationGroup", "ChangeOrganizationGroup", "RemoveOrganizationGroup", "AddOrganizationGroup"]
	createPermissionIds: ["AddOrganizationGroup", "EditOrganizationGroup"]
	editPermissionIds: ["ChangeOrganizationGroup", "EditOrganizationGroup"]
	deletePermissionIds: ["RemoveOrganizationGroup", "EditOrganizationGroup"]
	function removeItems(ids) {
		if (groupsPage.apiClient)
			groupsPage.apiClient.removeGroups(ids)
	}

	Connections {
		target: groupsPage.apiClient
		function onGroupsRemoved() {
			groupsPage.refresh()
		}
	}
}
