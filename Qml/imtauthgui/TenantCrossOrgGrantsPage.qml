// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantCrossOrgGrantsPage
 *
 * Cross-Org Grants tab of the TenantEditor.
 *
 * Lets a tenant creator/owner delegate scoped access from the current
 * (source) tenant to another (target) tenant via the ICrossOrgGrant
 * mechanism. Grants are listed, created and revoked through the abstract
 * TenantManagementApiClient contract (no direct SDL dependency here).
 */
ViewBase {
	id: grantsPage

	commandsPanelVisible: false
	contentColor: Style.baseColor

	readonly property var tenantData: grantsPage.model
	property var stateManager: null
	property var apiClient: null

	readonly property bool __canManage: grantsPage.stateManager
		&& (grantsPage.stateManager.isCreator || grantsPage.stateManager.isOwner)

	// Access level options. Index maps to the SDL CrossOrgAccessLevel tokens.
	readonly property var __accessLevelTokens: ["None", "Read", "Write", "Admin"]

	function updateGui() {
		grantsPage.__refreshGrants()
	}

	function __refreshGrants() {
		if (grantsPage.apiClient && grantsPage.tenantData && grantsPage.tenantData.m_id)
			grantsPage.apiClient.fetchCrossOrgGrants(grantsPage.tenantData.m_id)
	}

	function __clearForm() {
		targetTenantInput.text = ""
		relationshipInput.text = ""
		resourceScopeInput.text = ""
		descriptionInput.text = ""
		expiresInput.text = ""
		accessLevelInput.currentIndex = 1
	}

	function __createGrant() {
		if (!grantsPage.apiClient || !grantsPage.tenantData)
			return
		var targetTenantId = targetTenantInput.text.trim()
		var relationshipId = relationshipInput.text.trim()
		if (targetTenantId === "" || relationshipId === "") {
			ModalDialogManager.showInfoDialog(qsTr("Target tenant and relationship are required."))
			return
		}
		var levelIndex = accessLevelInput.currentIndex >= 0 ? accessLevelInput.currentIndex : 1
		var accessLevel = grantsPage.__accessLevelTokens[levelIndex]
		grantsPage.apiClient.createCrossOrgGrant(
			grantsPage.tenantData.m_id,
			targetTenantId,
			relationshipId,
			accessLevel,
			resourceScopeInput.text.trim(),
			"",
			descriptionInput.text.trim(),
			expiresInput.text.trim())
	}

	onVisibleChanged: {
		if (grantsPage.visible)
			grantsPage.__refreshGrants()
	}

	Component.onCompleted: grantsPage.__refreshGrants()

	Connections {
		target: grantsPage.apiClient

		function onCrossOrgGrantCreated(grantId) {
			PopupManager.addSuccessMessage(qsTr("Cross-org grant created successfully"), true)
			grantsPage.__clearForm()
			grantsPage.__refreshGrants()
		}

		function onCrossOrgGrantRevoked(grantId) {
			PopupManager.addSuccessMessage(qsTr("Cross-org grant revoked"), true)
			grantsPage.__refreshGrants()
		}
	}

	CustomScrollbar {
		id: grantsScrollbar
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: grantsFlickable.top
		anchors.bottom: grantsFlickable.bottom
		secondSize: Style.marginM
		targetItem: grantsFlickable
	}

	Flickable {
		id: grantsFlickable
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		contentHeight: grantsColumn.height + 2 * Style.marginXL
		clip: true

		Column {
			id: grantsColumn
			width: Style.sizeHintXXL
			spacing: Style.marginXL

			Column {
				width: parent.width
				spacing: Style.marginXS

				BaseText {
					text: qsTr("Cross-Org Grants")
					font.pixelSize: Style.fontSizeXL
					font.bold: true
					color: Style.textColor
				}

				BaseText {
					width: parent.width
					wrapMode: Text.WordWrap
					text: qsTr("Delegate scoped access from this tenant to another tenant. Access must be explicitly granted; parent tenants do not get implicit access.")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
			}

			Rectangle {
				width: parent.width
				height: 1
				color: Style.borderColor
			}

			// --- Create grant form ---
			GroupElementView {
				id: createGroup
				width: parent.width
				visible: grantsPage.__canManage

				TextInputElementView {
					id: targetTenantInput
					name: qsTr("Target Tenant ID")
					placeHolderText: qsTr("Tenant receiving access")
				}

				TextInputElementView {
					id: relationshipInput
					name: qsTr("Relationship ID")
					placeHolderText: qsTr("Associated tenant relationship")
				}

				ComboBoxElementView {
					id: accessLevelInput
					name: qsTr("Access Level")
					currentIndex: 1
					model: accessLevelModel
				}

				TextInputElementView {
					id: resourceScopeInput
					name: qsTr("Resource Scope")
					placeHolderText: qsTr("Optional — empty grants all resources")
				}

				TextInputElementView {
					id: descriptionInput
					name: qsTr("Description")
					placeHolderText: qsTr("Optional description")
				}

				TextInputElementView {
					id: expiresInput
					name: qsTr("Expires At")
					placeHolderText: qsTr("Optional ISO timestamp — empty for no expiry")
				}

				Button {
					text: qsTr("Create Grant")
					onClicked: grantsPage.__createGrant()
				}
			}

			// --- Existing grants ---
			BaseText {
				text: qsTr("Active Grants")
				font.pixelSize: Style.fontSizeL
				font.bold: true
				color: Style.textColor
			}

			BaseText {
				width: parent.width
				visible: !grantsList.count
				text: qsTr("No cross-org grants for this tenant.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			Column {
				id: grantsList
				width: parent.width
				spacing: Style.marginM

				property int count: grantsPage.apiClient && grantsPage.apiClient.crossOrgGrantsModel
					? grantsPage.apiClient.crossOrgGrantsModel.count
					: 0

				Repeater {
					model: grantsPage.apiClient ? grantsPage.apiClient.crossOrgGrantsModel : null

					delegate: Rectangle {
						width: grantsList.width
						height: grantRow.height + 2 * Style.marginM
						color: Style.alternateBaseColor
						radius: Style.radiusS
						border.color: Style.borderColor
						border.width: 1

						Row {
							id: grantRow
							anchors.left: parent.left
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							anchors.leftMargin: Style.marginM
							anchors.rightMargin: Style.marginM
							spacing: Style.marginM

							Column {
								width: parent.width - revokeButton.width - Style.marginM
								spacing: Style.marginXS

								BaseText {
									width: parent.width
									elide: Text.ElideRight
									text: qsTr("Target: %1").arg(model.targetTenantId || "")
									font.pixelSize: Style.fontSizeM
									color: Style.textColor
								}

								BaseText {
									width: parent.width
									elide: Text.ElideRight
									text: qsTr("Level: %1   Scope: %2")
										.arg(model.accessLevel || qsTr("None"))
										.arg((model.resourceScope && model.resourceScope !== "") ? model.resourceScope : qsTr("All"))
									font.pixelSize: Style.fontSizeS
									color: Style.inactiveTextColor
								}

								BaseText {
									width: parent.width
									visible: model.description && model.description !== ""
									elide: Text.ElideRight
									text: model.description || ""
									font.pixelSize: Style.fontSizeS
									color: Style.inactiveTextColor
								}
							}

							Button {
								id: revokeButton
								anchors.verticalCenter: parent.verticalCenter
								visible: grantsPage.__canManage && (model.isActive === undefined || model.isActive)
								text: qsTr("Revoke")
								onClicked: {
									if (grantsPage.apiClient)
										grantsPage.apiClient.revokeCrossOrgGrant(model.grantId || "")
								}
							}
						}
					}
				}
			}
		}
	}

	ListModel {
		id: accessLevelModel
		ListElement { name: "None" }
		ListElement { name: "Read" }
		ListElement { name: "Write" }
		ListElement { name: "Admin" }
	}
}
