// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtauthgui 1.0

/**
 * TenantPartyRolesPage
 *
 * The roles of the parties the contracts of the organization use: the system ones, given to every organization
 * and read only, and its own. A row opens the role in PartyRoleDialog. A role in use is archived, not removed:
 * the contracts keep it.
 */
SimpleCollectionPage {
	id: rolesPage

	entityName: qsTr("Role")
	entityNamePlural: qsTr("Roles of the parties")
	createButtonText: qsTr("+ New role")
	descriptionText: qsTr("The roles an organization has in a contract and towards another party. The system roles are given to every organization.")
	emptyText: qsTr("No roles yet.")
	deleteSingleTitle: qsTr("Archive role")
	deleteMultipleTitle: qsTr("Archive roles")

	// ContractsApiClient
	property var contractsApiClient: null

	readonly property string __tenantId: rolesPage.model && rolesPage.model.m_id ? String(rolesPage.model.m_id) : ""
	readonly property bool __canEditRoles: rolesPage.stateManager ? rolesPage.stateManager.canEditPartyRoles : false

	createPermissionIds: ["/OrganizationManagement/PartyRoles/Edit"]
	editPermissionIds: ["/OrganizationManagement/Contracts/View", "/OrganizationManagement/PartyRoles/Edit"]
	deletePermissionIds: ["/OrganizationManagement/PartyRoles/Edit"]

	columnIds: ["name", "kind", "pairedRole", "origin", "state"]
	columnNames: [qsTr("Name"), qsTr("Used for"), qsTr("Paired role"), qsTr("Owner"), qsTr("State")]

	listModel: rolesPage.__rows

	property var __rows: []

	function updateGui() {
		rolesPage.refresh()
	}

	// The archived roles are listed too: they are restored in the role
	function refresh() {
		if (rolesPage.contractsApiClient && rolesPage.__tenantId !== "")
			rolesPage.contractsApiClient.fetchPartyRoles(rolesPage.__tenantId, true)
	}

	Component.onCompleted: {
		rolesPage.refresh()
	}

	onVisibleChanged: {
		if (rolesPage.visible)
			rolesPage.refresh()
	}

	function __roleById(roleId) {
		var roles = rolesPage.contractsApiClient ? rolesPage.contractsApiClient.partyRoles : []
		for (var i = 0; i < roles.length; i++) {
			if (roles[i].roleId === roleId)
				return roles[i]
		}
		return null
	}

	function __kindText(role) {
		if (role.isPartyRole && role.isLinkRole)
			return qsTr("Party and link")
		return role.isPartyRole ? qsTr("Party") : qsTr("Link")
	}

	function __rowOf(role) {
		var paired = role.pairedRoleId ? rolesPage.__roleById(role.pairedRoleId) : null
		return {"id": role.roleId,
				"title": role.name,
				"name": role.name,
				"kind": rolesPage.__kindText(role),
				"pairedRole": paired ? paired.name : "",
				"origin": role.isSystem ? qsTr("System") : qsTr("Organization"),
				"state": role.isArchived ? qsTr("Archived") : qsTr("In use")}
	}

	// A system role stays: only the own roles of the organization are archived
	function removeItems(ids) {
		if (!rolesPage.contractsApiClient)
			return
		for (var i = 0; i < ids.length; i++) {
			var role = rolesPage.__roleById(ids[i])
			if (role && !role.isSystem && !role.isArchived)
				rolesPage.contractsApiClient.setPartyRoleArchived(rolesPage.__tenantId, role.roleId, true)
		}
	}

	function confirmRemoveItems(ids, itemTitle) {
		var removable = 0
		for (var i = 0; i < ids.length; i++) {
			var role = rolesPage.__roleById(ids[i])
			if (role && !role.isSystem && !role.isArchived)
				removable++
		}
		if (removable === 0) {
			PopupManager.addInfoMessage(qsTr("The system roles stay, and archived roles are archived already. Only the own roles of the organization are archived."), true)
			return
		}
		var message = ids.length === 1 && itemTitle && itemTitle !== ""
			? qsTr("Archive \"%1\"? The contracts using it keep it; it is not offered for new contracts.").arg(itemTitle)
			: qsTr("Archive %1 selected role(s)? The contracts using them keep them; they are not offered for new contracts.").arg(ids.length)
		ModalDialogManager.showConfirmationDialog(rolesPage.deleteSingleTitle, message, function(result) {
			if (result === Enums.yes) {
				rolesPage.removeItems(ids)
				if (rolesPage.selectionManager)
					rolesPage.selectionManager.clear()
			}
		})
	}

	Connections {
		target: rolesPage.contractsApiClient

		function onPartyRolesReceived() {
			var rows = []
			var roles = rolesPage.contractsApiClient.partyRoles || []
			for (var i = 0; i < roles.length; i++)
				rows.push(rolesPage.__rowOf(roles[i]))
			rolesPage.__rows = rows
		}

		function onCatalogItemSaved(catalog, itemId) {
			if (catalog !== "PartyRoles")
				return
			PopupManager.addSuccessMessage(qsTr("The role was saved"), true)
			rolesPage.refresh()
		}

		function onCommandFailed(command, errorMessage) {
			if (rolesPage.visible && command.indexOf("PartyRole") >= 0)
				PopupManager.addErrorMessage(errorMessage, true)
		}
	}

	editorDialogComponent: Component {
		PartyRoleDialog {
			tenantId: rolesPage.__tenantId
			contractsApiClient: rolesPage.contractsApiClient
			canEdit: rolesPage.__canEditRoles
		}
	}
}
