// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtauthgui 1.0

/**
 * TenantCrossOrgGrantsPage
 *
 * Cross-Org Grants tab of the TenantEditor.
 *
 * Displays grants via SimpleCollectionPage with a document manager
 * for full Create/Edit/Remove workflow.
 */
SimpleCollectionPage {
	id: grantsPage

	entityName: qsTr("Grant")
	entityNamePlural: qsTr("Cross Organization Grants")
	descriptionText: qsTr("Delegate scoped access from this organization to another organization.")
	showCreateButton: true
	documentNameFields: ["m_name", "m_targetTenantName"]

	documentManager: grantsPage.apiClient ? grantsPage.apiClient.crossOrgGrantDocumentManager : null
	objectTypeId: grantsPage.apiClient ? grantsPage.apiClient.crossOrgGrantObjectTypeId : ""
	dataProvider: grantsPage.apiClient ? grantsPage.apiClient.crossOrgGrantsListDataProvider : null
	managePermissionIds: ["/OrganizationManagement/Connections/Edit/Connect", "/OrganizationManagement/Connections/Edit/Remove"]
	createPermissionIds: ["/OrganizationManagement/Connections/Edit/Connect"]
	editPermissionIds: ["/OrganizationManagement/Connections/Edit/Connect"]
	deletePermissionIds: ["/OrganizationManagement/Connections/Edit/Remove"]

	columnIds: ["organization", "roles", "direction", "state", "expiresAt", "description"]
	columnNames: [qsTr("Organization"), qsTr("Roles"), qsTr("Direction"), qsTr("State"), qsTr("Expires"), qsTr("Description")]

	function createRow(item) {
		var targetTenant = grantsPage.parameterText(item, "targetTenant")
		var direction = grantsPage.parameterText(item, "direction")
		var directionText = ""
		if (direction === "from")
			directionText = qsTr("Received")
		else if (direction !== "")
			directionText = qsTr("Granted")
		var expiresAt = grantsPage.parameterText(item, "expiresAt")
		var expiresDate = new Date(expiresAt)
		if (expiresAt !== "" && !isNaN(expiresDate.getTime()))
			expiresAt = expiresDate.toLocaleDateString()
		var row = {}
		row.organization = targetTenant !== "" ? targetTenant : (item.title || "")
		row.roles = grantsPage.parameterText(item, "roles")
		row.direction = directionText
		row.state = grantsPage.parameterText(item, "isActive") === "false" ? qsTr("Inactive") : qsTr("Active")
		row.expiresAt = expiresAt
		row.description = item.description || ""
		return row
	}

	function removeItems(ids) {
		if (grantsPage.apiClient) {
			grantsPage.apiClient.removeCrossOrgGrants(ids)
		}
	}

	Connections {
		target: grantsPage.apiClient
		function onCrossOrgGrantsRemoved() {
			grantsPage.refresh()
		}
	}
}
