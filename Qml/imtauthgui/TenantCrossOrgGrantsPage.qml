// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtbaseImtBaseTypesSdl 1.0

/**
 * TenantCrossOrgGrantsPage
 *
 * Cross-Org Grants tab of the TenantEditor.
 *
 * Displays grants via TenantSimpleCollectionPage with a document manager
 * for full Create/Edit/Remove workflow.
 */
TenantSimpleCollectionPage {
	id: grantsPage

	entityName: qsTr("Grant")
	entityNamePlural: qsTr("Cross Organization Grants")
	descriptionText: qsTr("Delegate scoped access from this tenant to another tenant.")
	showCreateButton: true
	documentNameFields: ["m_name", "m_targetTenantName"]

	documentManager: grantsPage.apiClient ? grantsPage.apiClient.crossOrgGrantDocumentManager : null
	objectTypeId: grantsPage.apiClient ? grantsPage.apiClient.crossOrgGrantObjectTypeId : ""
	dataProvider: grantsPage.apiClient ? grantsPage.apiClient.crossOrgGrantsListDataProvider : null

	delegateComponent: Component {
		TenantCollectionItemDelegateBase {
			id: grantDelegate
			selectionManager: grantsPage.selectionManager
			collectionPage: grantsPage

			property TextParam __targetTenantParam: TextParam {}
			property TextParam __rolesParam: TextParam {}
			property TextParam __expiresAtParam: TextParam {}
			property TextParam __directionParam: TextParam {}

			readonly property string targetTenant: grantDelegate.__targetTenantParam.m_text || ""
			readonly property string roles: grantDelegate.__rolesParam.m_text || ""
			readonly property string expiresAt: grantDelegate.__expiresAtParam.m_text || ""
			readonly property string direction: grantDelegate.__directionParam.m_text || ""

			onItemParametersChanged: grantDelegate.__fillParams()

			function __fillParams() {
				if (!grantDelegate.itemParameters) return
				for (var i = 0; i < grantDelegate.itemParameters.length; i++) {
					var p = grantDelegate.itemParameters[i]
					var raw = p.data || ""
					if (p.id === "targetTenant") grantDelegate.__targetTenantParam.createFromJson(raw)
					else if (p.id === "roles") grantDelegate.__rolesParam.createFromJson(raw)
					else if (p.id === "expiresAt") grantDelegate.__expiresAtParam.createFromJson(raw)
					else if (p.id === "direction") grantDelegate.__directionParam.createFromJson(raw)
				}
			}

			Row {
				width: parent.width
				spacing: Style.marginM

				BaseText {
					text: grantDelegate.targetTenant !== "" ? grantDelegate.targetTenant : grantDelegate.itemTitle
					font.pixelSize: Style.fontSizeL
					font.bold: true
					color: Style.textColor
					width: parent.width - grantBadgesRow.width - Style.marginM
					elide: Text.ElideRight
				}

				Row {
					id: grantBadgesRow
					spacing: Style.marginXS

					StatusBadge {
						visible: grantDelegate.direction !== ""
						text: grantDelegate.direction === "from" ? qsTr("Received") : qsTr("Granted")
						badgeColor: grantDelegate.direction === "from" ? Style.selectedColor : Style.backgroundColor2
						badgeBorderWidth: 0
						textColor: Style.textColor
						fontPixelSize: Style.fontSizeXS
					}

					StatusBadge {
						visible: grantDelegate.expiresAt !== ""
						text: qsTr("Expires %1").arg(grantDelegate.expiresAt)
						badgeColor: Style.backgroundColor2
						badgeBorderWidth: 0
						textColor: Style.errorTextColor
						fontPixelSize: Style.fontSizeXS
					}
				}
			}

			BaseText {
				visible: grantDelegate.roles !== ""
				text: grantDelegate.roles
				font.pixelSize: Style.fontSizeS
				color: Style.secondColor
				elide: Text.ElideRight
				width: parent.width
			}

			BaseText {
				visible: grantDelegate.itemDescription !== ""
				text: grantDelegate.itemDescription
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
				elide: Text.ElideRight
				width: parent.width
			}
		}
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
