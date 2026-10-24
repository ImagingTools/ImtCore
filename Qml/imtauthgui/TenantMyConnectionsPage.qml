// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtauthgui 1.0

/**
 * TenantMyRelationshipsPage
 *
 * Displays the relationships of the organization.
 * Create and Edit open the RelationshipView, which sends a relationship proposal:
 * the relationship comes into force or changes when the partner approves it.
 * Remove ends the relationship.
 */
SimpleCollectionPage {
	id: relationshipsPage

	entityName: qsTr("Relationship")
	entityNamePlural: qsTr("Relationships")
	listHeaderTitle: qsTr("My Relationships")
	descriptionText: qsTr("Business relationships with other organizations. Creating or modifying a relationship requires approval from both parties.")
	showCreateButton: true

	dataProvider: relationshipsPage.apiClient ? relationshipsPage.apiClient.tenantRelationshipsListDataProvider : null
	customEditorComponent: relationshipEditorComp
	managePermissionIds: ["/OrganizationManagement/Connections/Edit/Connect", "/OrganizationManagement/Connections/Edit/Remove"]
	createPermissionIds: ["/OrganizationManagement/Connections/Edit/Connect"]
	editPermissionIds: ["/OrganizationManagement/Connections/Edit/Connect"]
	deletePermissionIds: ["/OrganizationManagement/Connections/Edit/Remove"]

	columnIds: ["partner", "partnerRole", "myRole", "status", "description"]
	columnNames: [qsTr("Partner"), qsTr("Partner role"), qsTr("Our role"), qsTr("Status"), qsTr("Description")]

	function createRow(item) {
		var partnerTenant = relationshipsPage.parameterText(item, "partnerTenant")
		var row = {}
		row.partner = partnerTenant !== "" ? partnerTenant : (item.title || "")
		row.partnerRole = relationshipsPage.parameterText(item, "role")
		row.myRole = relationshipsPage.parameterText(item, "myRole")
		row.status = relationshipsPage.parameterText(item, "status")
		row.description = item.description || ""
		return row
	}

	function removeItems(ids) {
		if (relationshipsPage.apiClient && relationshipsPage.model) {
			relationshipsPage.apiClient.removeTenantRelationship(relationshipsPage.model.m_id, ids[0])
		}
	}

	function __findRelationship(relationshipId) {
		var relationships = relationshipsPage.apiClient ? relationshipsPage.apiClient.tenantRelationshipsModel : null
		if (!relationships) {
			return null
		}
		for (var i = 0; i < relationships.count; ++i) {
			if (relationships.get(i).id === relationshipId) {
				return relationships.get(i)
			}
		}
		return null
	}

	function updateGui() {
		if (relationshipsPage.apiClient && relationshipsPage.model && relationshipsPage.model.m_id) {
			relationshipsPage.apiClient.fetchTenantRelationships(relationshipsPage.model.m_id)
		}
	}

	Component.onCompleted: {
		relationshipsPage.updateGui()
	}

	Connections {
		target: relationshipsPage.apiClient
		function onTenantRelationshipRemoved() {
			relationshipsPage.refresh()
			relationshipsPage.updateGui()
		}
		function onRelationshipProposalCreated() {
			PopupManager.addSuccessMessage(qsTr("The proposal was sent to the partner organization"), true)
			relationshipsPage.popEditor()
		}
		function onRelationshipProposalApproved(relationshipId) {
			relationshipsPage.refresh()
			relationshipsPage.updateGui()
		}
	}

	Component {
		id: relationshipEditorComp

		RelationshipView {
			apiClient: relationshipsPage.apiClient
			tenantData: relationshipsPage.model
			partnerName: relationshipsPage.__isCreating ? "" : relationshipsPage.__editItemTitle()
			relationship: relationshipsPage.__isCreating ? null : relationshipsPage.__findRelationship(relationshipsPage.__editItemId)
		}
	}

	function __editItemTitle() {
		var item = relationshipsPage.__findItemById(relationshipsPage.__editItemId)
		return item ? (item.title || "") : ""
	}
}
