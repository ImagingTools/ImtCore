// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtauthgui 1.0

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
	entityNamePlural: qsTr("Cross-Org Grants")
	descriptionText: qsTr("Delegate scoped access from this tenant to another tenant.")
	showCreateButton: true
	documentNameFields: ["m_name", "m_targetTenantName"]

		documentManager: grantsPage.apiClient ? grantsPage.apiClient.crossOrgGrantDocumentManager : null
	objectTypeId: grantsPage.apiClient ? grantsPage.apiClient.crossOrgGrantObjectTypeId : ""
	dataProvider: grantsPage.apiClient ? grantsPage.apiClient.crossOrgGrantsListDataProvider : null

	// Use custom delegate to fetch targetTenantName instead of 'name' if dataProvider only returns ID for title.
	// But since we can't easily fetch document fields synchronously from list items using just item.id,
	// we will try to use the itemTitle which will be standard ID for now, and see if there's any other field inside modelData (e.g. description or typeId). 
	// Wait, does 'modelData' contain 'title' that IS the tenant name? 
	// Let's create a custom delegateComponent to display the ID as the description or handle it explicitly if needed.
	// Since target tenant name is not available in FilterableSelectGqlDataProvider items unless configured on server.
	// The issue says "сейчас как Name отображается ID, а нужно отображать Tenant Name (из Target CrossOrgGrant)"
	// To fix this globally, we should ideally fix it on the server (meta info delegate). But since there is none, 
	// we can use documentManager to resolve the name dynamically in the UI!?

	delegateComponent: Component {
		Rectangle {
			id: itemDelegateRoot
			width: ListView.view ? ListView.view.width : 0
			height: Style.controlHeightL + Style.marginL

			property string itemId: modelData.id || ""

			// Try to resolve the name dynamically using the page's resolveDocumentName function (which loads from documentManager cache)
			// However, this might trigger a network request per item if not cached, which is fine for small lists.
			// A better fallback is item.title if resolve gives nothing initially.
			property string itemTitle: {
				var n = grantsPage.resolveDocumentName(itemId)
				if (n !== "" && n !== itemId) return n
				return modelData.title || itemId || ""
			}
			property string itemDescription: modelData.description || "id: " + itemId
			property bool isSelected: grantsPage.selectionManager ? grantsPage.selectionManager.isSelected(itemId) : false

			// Component to trigger background load if needed:
			Component.onCompleted: {
				// Force document manager to load the view so resolveDocumentName works
				if (grantsPage.documentManager) {
					grantsPage.documentManager.getDocumentViewInstance(itemId, "")
				}
			}

			color: isSelected ? Style.selectedColor
							  : itemMouseArea.containsMouse ? Style.buttonHoverColor
															: "transparent"
			MouseArea {
				id: itemMouseArea
				anchors.fill: parent
				hoverEnabled: true
				cursorShape: Qt.PointingHandCursor
				onDoubleClicked: {
					if (grantsPage.__canManage)
						grantsPage.__openEdit(itemDelegateRoot.itemId, itemDelegateRoot.itemTitle, itemDelegateRoot.itemDescription)
				}
			}

			Row {
				anchors.left: parent.left
				anchors.right: moreButton.left
				anchors.verticalCenter: parent.verticalCenter
				anchors.leftMargin: Style.marginM
				anchors.rightMargin: Style.marginM
				spacing: Style.marginM

				CheckBox {
					anchors.verticalCenter: parent.verticalCenter
					height: Style.itemSizeS
					width: Style.itemSizeS
					checkState: itemDelegateRoot.isSelected ? Qt.Checked : Qt.Unchecked
					onCheckStateChanged: {
						var shouldBeSelected = (checkState === Qt.Checked)
						var currentlySelected = grantsPage.selectionManager.isSelected(itemDelegateRoot.itemId)
						if (shouldBeSelected !== currentlySelected)
							grantsPage.selectionManager.toggleSelect(itemDelegateRoot.itemId)
					}
				}

				Column {
					anchors.verticalCenter: parent.verticalCenter
					spacing: Style.marginXS
					width: parent.width - Style.itemSizeS - parent.spacing

					BaseText {
						text: itemDelegateRoot.itemTitle
						font.pixelSize: Style.fontSizeL
						font.bold: true
						color: Style.textColor
					}

					BaseText {
						visible: itemDelegateRoot.itemDescription !== ""
						text: itemDelegateRoot.itemDescription
						font.pixelSize: Style.fontSizeM
						color: Style.inactiveTextColor
						elide: Text.ElideRight
						width: parent.width
					}
				}
			}

			Rectangle {
				id: moreButton
				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				anchors.verticalCenter: parent.verticalCenter
				width: Style.controlHeightM
				height: Style.controlHeightM
				radius: Style.controlHeightM / 2
				color: moreButtonMA.containsMouse ? Style.buttonHoverColor : "transparent"
				visible: grantsPage.__canManage && (itemMouseArea.containsMouse || itemDelegateRoot.isSelected || moreButtonMA.containsMouse)

				Text {
					anchors.centerIn: parent
					text: "\u2026"
					font.pixelSize: Style.fontSizeL
					color: Style.textColor
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
				}

				MouseArea {
					id: moreButtonMA
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: Qt.PointingHandCursor
					onClicked: itemMenu.popup()
				}
			}

			Menu {
				id: itemMenu
				MenuItem {
					text: qsTr("Edit")
					enabled: grantsPage.__canManage
					onTriggered: grantsPage.__openEdit(itemDelegateRoot.itemId, itemDelegateRoot.itemTitle, itemDelegateRoot.itemDescription)
				}
				MenuItem {
					text: qsTr("Delete")
					enabled: grantsPage.__canManage
					onTriggered: {
						ModalDialogManager.showConfirmationDialog(
									grantsPage.__deleteSingleTitle,
									qsTr("Are you sure you want to delete \"%1\"? This action cannot be undone.").arg(itemDelegateRoot.itemTitle),
									function(result) {
										if (result === Enums.yes)
											grantsPage.removeItem(itemDelegateRoot.itemId)
									}
									)
					}
				}
			}

			Rectangle {
				anchors.bottom: parent.bottom
				anchors.left: parent.left
				anchors.right: parent.right
				height: 1
				color: Style.borderColor
				opacity: 0.5
			}
		}
	}

		function removeItem(id) {
		if (grantsPage.documentManager) {
			grantsPage.documentManager.removeDocument(id)
		}
	}

	Connections {
		target: grantsPage.apiClient
		function onCrossOrgGrantRevoked(grantId) {
			grantsPage.refresh()
		}
	}
}
