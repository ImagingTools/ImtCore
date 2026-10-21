// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtauthgui 1.0

/**
 * RelationshipView
 *
 * Editor of a relationship proposal. A relationship comes into force or changes
 * only when the partner organization approves the proposal sent from here.
 *
 * The fields are shown relative to the current organization:
 *   - "Partner Organization" (the other side)
 *   - "My Role" (role of the current organization)
 *   - "Partner Role" (role of the partner organization)
 *
 * For an existing relationship set \c relationship (an entry of
 * apiClient.tenantRelationshipsModel) and \c partnerName.
 */
ViewBase {
	id: container

	anchors.fill: parent
	contentColor: Style.baseColor
	commandsPanelVisible: false

	property var apiClient: null
	property var tenantData: null
	property var relationship: null
	property string partnerName: ""

	property string __selectedPartnerTenantId: ""
	property string __selectedPartnerTenantName: ""
	property bool sending: false

	readonly property bool __isExistingRelationship: container.relationship !== null
	readonly property string __currentTenantId: container.tenantData ? (container.tenantData.m_id || "") : ""
	// In an existing relationship the current organization is either its source or its target.
	readonly property bool __isSource: !container.__isExistingRelationship || container.relationship.sourceTenantId === container.__currentTenantId
	readonly property var __roles: ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]

	Component.onCompleted: {
		for (var i = 0; i < container.__roles.length; i++) {
			var idx = roleModel.insertNewItem()
			roleModel.setData("id", container.__roles[i], idx)
			roleModel.setData("name", container.__roles[i], idx)
		}
		myRoleCB.currentIndex = 2
		partnerRoleCB.currentIndex = 2
		container.updateGui()
	}

	function updateGui() {
		if (!container.__isExistingRelationship) {
			return
		}

		container.__selectedPartnerTenantId = container.__isSource ? container.relationship.targetTenantId : container.relationship.sourceTenantId
		container.__selectedPartnerTenantName = container.partnerName

		var myRole = container.__isSource ? container.relationship.sourceRole : container.relationship.targetRole
		var partnerRole = container.__isSource ? container.relationship.targetRole : container.relationship.sourceRole
		myRoleCB.currentIndex = Math.max(container.__roles.indexOf(myRole), 0)
		partnerRoleCB.currentIndex = Math.max(container.__roles.indexOf(partnerRole), 0)
		relScopeInput.text = container.relationship.scope || ""
		relDescriptionInput.text = container.relationship.description || ""
	}

	function submitProposal() {
		if (container.__selectedPartnerTenantId === "") {
			PopupManager.addWarningMessage(qsTr("Select the partner organization."), true)
			return
		}
		if (!container.apiClient) {
			return
		}

		var myRole = container.__roles[myRoleCB.currentIndex >= 0 ? myRoleCB.currentIndex : 2]
		var partnerRole = container.__roles[partnerRoleCB.currentIndex >= 0 ? partnerRoleCB.currentIndex : 2]

		// Roles are proposed for the relationship's source and target; a new relationship has the current organization as its source.
		container.sending = true
		container.apiClient.createRelationshipProposal(
			container.__currentTenantId,
			container.__selectedPartnerTenantId,
			container.__isExistingRelationship ? "Update" : "Create",
			container.__isExistingRelationship ? container.relationship.id : "",
			container.__isSource ? myRole : partnerRole,
			container.__isSource ? partnerRole : myRole,
			relScopeInput.text.trim(),
			relDescriptionInput.text.trim())
	}

	Connections {
		target: container.apiClient

		function onRelationshipProposalCreated() {
			container.sending = false
		}

		function onRequestFailed(message) {
			container.sending = false
		}
	}

	CustomScrollbar {
		id: scrollbar
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: flickable.top
		anchors.bottom: flickable.bottom
		secondSize: Style.marginM
		targetItem: flickable
	}

	Flickable {
		id: flickable
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.horizontalCenter: parent.horizontalCenter
		width: container.width
		contentWidth: bodyColumn.width
		contentHeight: bodyColumn.height + 2 * Style.marginXL
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: bodyColumn
			width: container.width
			spacing: Style.marginXL

			GroupElementView {
				id: generalGroup
				width: parent.width

				ElementView {
					name: qsTr("Partner Organization")

					controlComp: Component {
						Row {
							id: partnerTenantRow
							spacing: Style.marginM

							BaseText {
								anchors.verticalCenter: parent.verticalCenter
								text: container.__selectedPartnerTenantName
									  || container.__selectedPartnerTenantId
									  || qsTr("Select organization...")
								color: container.__selectedPartnerTenantId ? Style.textColor : Style.inactiveTextColor
								font.pixelSize: Style.fontSizeM
							}

							Button {
								text: qsTr("Select")
								visible: !container.__isExistingRelationship
								onClicked: {
									var point = partnerTenantRow.mapToItem(null, 0, partnerTenantRow.height)
									ModalDialogManager.openDialog(tenantSelectComp, {"x": point.x, "y": point.y})
								}
							}
						}
					}
				}

				ComboBoxElementView {
					id: myRoleCB
					name: qsTr("My Role")
					model: roleModel
				}

				ComboBoxElementView {
					id: partnerRoleCB
					name: qsTr("Partner Role")
					model: roleModel
				}

				TextInputElementView {
					id: relScopeInput
					name: qsTr("Scope")
					placeHolderText: qsTr("Optional — empty applies to all resources")
				}

				TextInputElementView {
					id: relDescriptionInput
					name: qsTr("Description")
					placeHolderText: qsTr("Optional description")
				}
			}

			BaseText {
				width: parent.width
				wrapMode: Text.WordWrap
				text: container.__isExistingRelationship
					  ? qsTr("The changes take effect when the partner organization approves them.")
					  : qsTr("The relationship comes into force when the partner organization approves it.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			Button {
				variant: "primary"
				text: container.sending
					  ? qsTr("Sending...")
					  : (container.__isExistingRelationship ? qsTr("Propose Changes") : qsTr("Send Proposal"))
				enabled: !container.sending
				onClicked: {
					container.submitProposal()
				}
			}
		}
	}

	Component {
		id: tenantSelectComp

		FilterableSelectPopup {
			dataProvider: container.apiClient ? container.apiClient.connectionsDataProvider : null
			filterPlaceholder: qsTr("Select organization...")
			emptyText: qsTr("No connected organizations. Connect one under Relationships first.")
			preselectedIds: container.__selectedPartnerTenantId ? [container.__selectedPartnerTenantId] : []

			onItemSelected: {
				if (itemId === container.__currentTenantId) {
					PopupManager.addWarningMessage(qsTr("Choose another organization. Current and partner organizations must be different."), true)
					return
				}
				container.__selectedPartnerTenantId = itemId
				container.__selectedPartnerTenantName = dataProvider ? dataProvider.getSelectedItemText(itemId) : ""
			}
		}
	}

	TreeItemModel {
		id: roleModel
	}
}
