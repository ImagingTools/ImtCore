// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0
import imtbaseImtBaseTypesSdl 1.0

/**
 * TenantMyRelationshipsPage
 *
 * Displays confirmed relationships for the tenant.
 * Uses standard TenantSimpleCollectionPage with FilterableSelectGqlDataProvider
 * (same mechanism as TenantRolesPage). Supports Remove/Edit/Create.
 * Create opens the RelationshipView which creates a relationship proposal
 * (bilateral approval required).
 */
TenantSimpleCollectionPage {
	id: relationshipsPage

	entityName: qsTr("Relationship")
	entityNamePlural: qsTr("My Relationships")
	descriptionText: qsTr("Business relationships with other organizations. Creating or modifying a relationship requires approval from both parties.")
	showCreateButton: true
	documentNameFields: ["m_targetTenantName"]

	documentManager: relationshipsPage.apiClient ? relationshipsPage.apiClient.relationshipDocumentManager : null
	objectTypeId: relationshipsPage.apiClient ? relationshipsPage.apiClient.relationshipObjectTypeId : ""
	dataProvider: relationshipsPage.apiClient ? relationshipsPage.apiClient.tenantRelationshipsListDataProvider : null

	delegateComponent: Component {
		TenantCollectionItemDelegateBase {
			id: relDelegate
			selectionManager: relationshipsPage.selectionManager
			collectionPage: relationshipsPage

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
					font.pixelSize: Style.fontSizeL
					font.bold: true
					color: Style.textColor
					width: parent.width - statusBadgeRow.width - Style.marginM
					elide: Text.ElideRight
				}

				Row {
					id: statusBadgeRow
					spacing: Style.marginXS

					StatusBadge {
						text: relDelegate.status
						badgeColor: relDelegate.status === "Active" ? Style.selectedColor : Style.backgroundColor2
						badgeBorderWidth: 0
						textColor: relDelegate.status === "Pending" ? Style.errorTextColor : Style.textColor
						fontPixelSize: Style.fontSizeXS
					}
				}
			}

			Row {
				width: parent.width
				spacing: Style.marginXS
				visible: relDelegate.myTenant !== "" || relDelegate.partnerTenant !== ""

				StatusBadge {
					visible: relDelegate.myTenant !== ""
					text: relDelegate.myTenant + (relDelegate.myRole !== "" ? " (" + relDelegate.myRole + ")" : "")
					badgeColor: Style.backgroundColor2
					badgeBorderWidth: 0
					textColor: Style.textColor
					fontPixelSize: Style.fontSizeXS
				}

				BaseText {
					text: "\u2194"
					font.pixelSize: Style.fontSizeM
					color: Style.inactiveTextColor
					visible: relDelegate.myTenant !== "" && relDelegate.partnerTenant !== ""
				}

				StatusBadge {
					visible: relDelegate.partnerTenant !== ""
					text: relDelegate.partnerTenant + (relDelegate.role !== "" ? " (" + relDelegate.role + ")" : "")
					badgeColor: Style.baseColor
					badgeBorderColor: Style.secondColor
					textColor: Style.secondColor
					fontPixelSize: Style.fontSizeXS
				}
			}

			BaseText {
				visible: relDelegate.itemDescription !== ""
				text: relDelegate.itemDescription
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
				elide: Text.ElideRight
				width: parent.width
			}
		}
	}

	function removeItems(ids) {
		if (relationshipsPage.apiClient && relationshipsPage.tenantData) {
			relationshipsPage.apiClient.removeTenantRelationship(relationshipsPage.tenantData.m_id, ids[0])
		}
	}

	Connections {
		target: relationshipsPage.apiClient
		function onTenantRelationshipRemoved() {
			relationshipsPage.refresh()
		}
	}
}
