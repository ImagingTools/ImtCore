// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantRelationshipsPage
 *
 * Relationships tab of the TenantEditor.
 *
 * Lets a tenant creator/owner manage asymmetric relationships between the
 * current (source) tenant and other tenants. Each relationship carries a
 * distinct role for the source and the target side, an optional resource
 * scope and a validity window. Relationships are listed, created and removed
 * through the abstract TenantManagementApiClient contract.
 */
ViewBase {
	id: relationshipsPage

	commandsPanelVisible: false
	contentColor: Style.baseColor

	readonly property var tenantData: relationshipsPage.model
	property var stateManager: null
	property var apiClient: null

	readonly property bool __canManage: relationshipsPage.stateManager
		&& (relationshipsPage.stateManager.isCreator || relationshipsPage.stateManager.isOwner)

	// Role options. Index maps to the SDL TenantRelationshipRole tokens.
	readonly property var __roleTokens: ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]

	function updateGui() {
		relationshipsPage.__refresh()
	}

	function __refresh() {
		if (relationshipsPage.apiClient && relationshipsPage.tenantData && relationshipsPage.tenantData.m_id)
			relationshipsPage.apiClient.fetchTenantRelationships(relationshipsPage.tenantData.m_id)
	}

	function __clearForm() {
		targetTenantInput.text = ""
		scopeInput.text = ""
		validFromInput.text = ""
		validUntilInput.text = ""
		descriptionInput.text = ""
		sourceRoleInput.currentIndex = 2
		targetRoleInput.currentIndex = 2
	}

	function __createRelationship() {
		if (!relationshipsPage.apiClient || !relationshipsPage.tenantData)
			return
		var targetTenantId = targetTenantInput.text.trim()
		if (targetTenantId === "") {
			ModalDialogManager.showInfoDialog(qsTr("Target tenant is required."))
			return
		}
		var sourceIndex = sourceRoleInput.currentIndex >= 0 ? sourceRoleInput.currentIndex : 2
		var targetIndex = targetRoleInput.currentIndex >= 0 ? targetRoleInput.currentIndex : 2
		relationshipsPage.apiClient.addTenantRelationship(
			relationshipsPage.tenantData.m_id,
			targetTenantId,
			relationshipsPage.__roleTokens[sourceIndex],
			relationshipsPage.__roleTokens[targetIndex],
			scopeInput.text.trim(),
			validFromInput.text.trim(),
			validUntilInput.text.trim(),
			descriptionInput.text.trim())
	}

	onVisibleChanged: {
		if (relationshipsPage.visible)
			relationshipsPage.__refresh()
	}

	Component.onCompleted: relationshipsPage.__refresh()

	Connections {
		target: relationshipsPage.apiClient

		function onTenantRelationshipAdded(relationshipId) {
			PopupManager.addSuccessMessage(qsTr("Relationship created successfully"), true)
			relationshipsPage.__clearForm()
			relationshipsPage.__refresh()
		}

		function onTenantRelationshipRemoved(relationshipId) {
			PopupManager.addSuccessMessage(qsTr("Relationship removed"), true)
			relationshipsPage.__refresh()
		}
	}

	CustomScrollbar {
		id: relationshipsScrollbar
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: relationshipsFlickable.top
		anchors.bottom: relationshipsFlickable.bottom
		secondSize: Style.marginM
		targetItem: relationshipsFlickable
	}

	Flickable {
		id: relationshipsFlickable
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		contentHeight: relationshipsColumn.height + 2 * Style.marginXL
		clip: true

		Column {
			id: relationshipsColumn
			width: Style.sizeHintXXL
			spacing: Style.marginXL

			Column {
				width: parent.width
				spacing: Style.marginXS

				BaseText {
					text: qsTr("Tenant Relationships")
					font.pixelSize: Style.fontSizeXL
					font.bold: true
					color: Style.textColor
				}

				BaseText {
					width: parent.width
					wrapMode: Text.WordWrap
					text: qsTr("Define asymmetric relationships between this tenant and others. Each side has its own role, with an optional scope and validity window.")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
			}

			Rectangle {
				width: parent.width
				height: 1
				color: Style.borderColor
			}

			// --- Create relationship form ---
			GroupElementView {
				id: createGroup
				width: parent.width
				visible: relationshipsPage.__canManage

				TextInputElementView {
					id: targetTenantInput
					name: qsTr("Target Tenant ID")
					placeHolderText: qsTr("Tenant on the other side")
				}

				ComboBoxElementView {
					id: sourceRoleInput
					name: qsTr("Source Role")
					currentIndex: 2
					model: roleModel
				}

				ComboBoxElementView {
					id: targetRoleInput
					name: qsTr("Target Role")
					currentIndex: 2
					model: roleModel
				}

				TextInputElementView {
					id: scopeInput
					name: qsTr("Scope")
					placeHolderText: qsTr("Optional — empty applies to all resources")
				}

				TextInputElementView {
					id: validFromInput
					name: qsTr("Valid From")
					placeHolderText: qsTr("Optional ISO timestamp")
				}

				TextInputElementView {
					id: validUntilInput
					name: qsTr("Valid Until")
					placeHolderText: qsTr("Optional ISO timestamp — empty for no expiry")
				}

				TextInputElementView {
					id: descriptionInput
					name: qsTr("Description")
					placeHolderText: qsTr("Optional description")
				}

				Button {
					text: qsTr("Create Relationship")
					onClicked: relationshipsPage.__createRelationship()
				}
			}

			// --- Existing relationships ---
			BaseText {
				text: qsTr("Active Relationships")
				font.pixelSize: Style.fontSizeL
				font.bold: true
				color: Style.textColor
			}

			BaseText {
				width: parent.width
				visible: !relationshipsList.count
				text: qsTr("No relationships for this tenant.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			Column {
				id: relationshipsList
				width: parent.width
				spacing: Style.marginM

				property int count: relationshipsPage.apiClient && relationshipsPage.apiClient.tenantRelationshipsModel
					? relationshipsPage.apiClient.tenantRelationshipsModel.count
					: 0

				Repeater {
					model: relationshipsPage.apiClient ? relationshipsPage.apiClient.tenantRelationshipsModel : null

					delegate: Rectangle {
						width: relationshipsList.width
						height: relationshipRow.height + 2 * Style.marginM
						color: Style.alternateBaseColor
						radius: Style.radiusS
						border.color: Style.borderColor
						border.width: 1

						Row {
							id: relationshipRow
							anchors.left: parent.left
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							anchors.leftMargin: Style.marginM
							anchors.rightMargin: Style.marginM
							spacing: Style.marginM

							Column {
								width: parent.width - removeButton.width - Style.marginM
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
									text: qsTr("Source role: %1   Target role: %2")
										.arg(model.sourceRole || model.role || qsTr("Partner"))
										.arg(model.targetRole || qsTr("Partner"))
									font.pixelSize: Style.fontSizeS
									color: Style.inactiveTextColor
								}

								BaseText {
									width: parent.width
									elide: Text.ElideRight
									text: qsTr("Scope: %1")
										.arg((model.scope && model.scope !== "") ? model.scope : qsTr("All"))
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
								id: removeButton
								anchors.verticalCenter: parent.verticalCenter
								visible: relationshipsPage.__canManage
								text: qsTr("Remove")
								onClicked: {
									if (relationshipsPage.apiClient)
										relationshipsPage.apiClient.removeTenantRelationship(
											relationshipsPage.tenantData ? relationshipsPage.tenantData.m_id : "",
											model.relationshipId || "")
								}
							}
						}
					}
				}
			}
		}
	}

	ListModel {
		id: roleModel
		ListElement { name: "Parent" }
		ListElement { name: "Child" }
		ListElement { name: "Partner" }
		ListElement { name: "Supplier" }
		ListElement { name: "Customer" }
		ListElement { name: "Affiliate" }
	}
}
