// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtauthgui 1.0
import imtbaseImtBaseTypesSdl 1.0

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

	delegateComponent: Component {
		SimpleCollectionItemDelegateBase {
			id: relDelegate
			selectionManager: relationshipsPage.selectionManager
			actionHandler: relationshipsPage

			property TextParam __partnerTenantParam: TextParam {}
			property TextParam __myTenantParam: TextParam {}
			property TextParam __roleParam: TextParam {}
			property TextParam __myRoleParam: TextParam {}
			property TextParam __statusParam: TextParam {}

			readonly property string partnerTenant: relDelegate.__partnerTenantParam.m_text || ""
			readonly property string myTenant: relDelegate.__myTenantParam.m_text || ""
			readonly property string role: relDelegate.__roleParam.m_text || ""
			readonly property string myRole: relDelegate.__myRoleParam.m_text || ""
			readonly property string status: relDelegate.__statusParam.m_text || ""

			onItemParametersChanged: relDelegate.__fillParams()

			function __connectionSummary() {
				var myPart = relDelegate.myTenant + (relDelegate.myRole !== "" ? " (" + relDelegate.myRole + ")" : "")
				var partnerPart = relDelegate.partnerTenant + (relDelegate.role !== "" ? " (" + relDelegate.role + ")" : "")
				if (relDelegate.myTenant !== "" && relDelegate.partnerTenant !== "")
					return myPart + " \u2194 " + partnerPart
				return relDelegate.myTenant !== "" ? myPart : partnerPart
			}

			function __fillParams() {
				if (!relDelegate.itemParameters) return
				for (var i = 0; i < relDelegate.itemParameters.length; i++) {
					var p = relDelegate.itemParameters[i]
					var raw = p.data || ""
					if (p.id === "partnerTenant") relDelegate.__partnerTenantParam.createFromJson(raw)
					else if (p.id === "myTenant") relDelegate.__myTenantParam.createFromJson(raw)
					else if (p.id === "role") relDelegate.__roleParam.createFromJson(raw)
					else if (p.id === "myRole") relDelegate.__myRoleParam.createFromJson(raw)
					else if (p.id === "status") relDelegate.__statusParam.createFromJson(raw)
				}
			}

			Row {
				width: parent.width
				spacing: Style.marginM

				BaseText {
					text: relDelegate.partnerTenant !== "" ? relDelegate.partnerTenant : relDelegate.itemTitle
					font.pixelSize: Style.fontSizeM
					font.bold: true
					color: Style.textColor
					width: parent.width - relInfoRow.width - Style.marginM
					elide: Text.ElideRight
				}

				Row {
					id: relInfoRow
					spacing: Style.marginS

					BaseText {
						visible: relDelegate.myTenant !== "" || relDelegate.partnerTenant !== ""
						text: relDelegate.__connectionSummary()
						font.pixelSize: Style.fontSizeS
						color: Style.subtitleColor
						anchors.verticalCenter: parent.verticalCenter
					}

					StatusBadge {
						text: relDelegate.status
						badgeColor: Style.neutralSubtleColor
						badgeBorderWidth: 0
						textColor: relDelegate.status === "Active" ? Style.successColor : Style.subtitleColor
						fontPixelSize: Style.fontSizeXS
					}
				}
			}

			BaseText {
				visible: relDelegate.itemDescription !== ""
				text: relDelegate.itemDescription
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
				elide: Text.ElideRight
				width: parent.width
			}
		}
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
