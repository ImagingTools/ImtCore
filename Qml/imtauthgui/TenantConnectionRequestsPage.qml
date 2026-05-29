// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantConnectionRequestsPage
 *
 * Connections tab of the TenantEditor.
 *
 * Implements the tenant discovery / bootstrap workflow: a creator/owner can
 * send a connection request to another tenant (by external identifier),
 * generate a one-time connect code that any tenant can redeem, redeem a code
 * received out-of-band, and accept / reject / revoke pending requests. On
 * acceptance an asymmetric relationship is established between both tenants.
 * All operations go through the abstract TenantManagementApiClient contract.
 */
ViewBase {
	id: connectionsPage

	commandsPanelVisible: false
	contentColor: Style.baseColor

	readonly property var tenantData: connectionsPage.model
	property var stateManager: null
	property var apiClient: null

	readonly property bool __canManage: connectionsPage.stateManager
		&& (connectionsPage.stateManager.isCreator || connectionsPage.stateManager.isOwner)

	// Role options. Index maps to the SDL TenantRelationshipRole tokens.
	readonly property var __roleTokens: ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]

	function updateGui() {
		connectionsPage.__refresh()
	}

	function __refresh() {
		if (connectionsPage.apiClient && connectionsPage.tenantData && connectionsPage.tenantData.m_id)
			connectionsPage.apiClient.fetchConnectionRequests(connectionsPage.tenantData.m_id)
	}

	function __clearForm() {
		targetIdentifierInput.text = ""
		messageInput.text = ""
		expiresInput.text = ""
		sourceRoleInput.currentIndex = 2
		targetRoleInput.currentIndex = 2
	}

	function __createRequest() {
		if (!connectionsPage.apiClient || !connectionsPage.tenantData)
			return
		var targetIdentifier = targetIdentifierInput.text.trim()
		if (targetIdentifier === "") {
			ModalDialogManager.showInfoDialog(qsTr("Target identifier is required."))
			return
		}
		var sourceIndex = sourceRoleInput.currentIndex >= 0 ? sourceRoleInput.currentIndex : 2
		var targetIndex = targetRoleInput.currentIndex >= 0 ? targetRoleInput.currentIndex : 2
		connectionsPage.apiClient.createConnectionRequest(
			connectionsPage.tenantData.m_id,
			targetIdentifier,
			connectionsPage.__roleTokens[sourceIndex],
			connectionsPage.__roleTokens[targetIndex],
			messageInput.text.trim(),
			expiresInput.text.trim())
	}

	function __createCode() {
		if (!connectionsPage.apiClient || !connectionsPage.tenantData)
			return
		var sourceIndex = sourceRoleInput.currentIndex >= 0 ? sourceRoleInput.currentIndex : 2
		var targetIndex = targetRoleInput.currentIndex >= 0 ? targetRoleInput.currentIndex : 2
		connectionsPage.apiClient.createConnectCode(
			connectionsPage.tenantData.m_id,
			connectionsPage.__roleTokens[sourceIndex],
			connectionsPage.__roleTokens[targetIndex],
			messageInput.text.trim(),
			expiresInput.text.trim())
	}

	function __redeemCode() {
		if (!connectionsPage.apiClient || !connectionsPage.tenantData)
			return
		var code = redeemCodeInput.text.trim()
		if (code === "") {
			ModalDialogManager.showInfoDialog(qsTr("Connect code is required."))
			return
		}
		connectionsPage.apiClient.acceptConnectCode(code, connectionsPage.tenantData.m_id)
	}

	onVisibleChanged: {
		if (connectionsPage.visible)
			connectionsPage.__refresh()
	}

	Component.onCompleted: connectionsPage.__refresh()

	Connections {
		target: connectionsPage.apiClient

		function onConnectionRequestCreated(requestId) {
			PopupManager.addSuccessMessage(qsTr("Connection request sent"), true)
			connectionsPage.__clearForm()
			connectionsPage.__refresh()
		}

		function onConnectCodeCreated(requestId, connectCode) {
			ModalDialogManager.showInfoDialog(qsTr("Connect code created: %1").arg(connectCode))
			connectionsPage.__clearForm()
			connectionsPage.__refresh()
		}

		function onConnectionRequestAccepted(requestId) {
			PopupManager.addSuccessMessage(qsTr("Connection request accepted"), true)
			redeemCodeInput.text = ""
			connectionsPage.__refresh()
		}

		function onConnectionRequestRejected(requestId) {
			PopupManager.addSuccessMessage(qsTr("Connection request rejected"), true)
			connectionsPage.__refresh()
		}

		function onConnectionRequestRevoked(requestId) {
			PopupManager.addSuccessMessage(qsTr("Connection request revoked"), true)
			connectionsPage.__refresh()
		}
	}

	CustomScrollbar {
		id: connectionsScrollbar
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: connectionsFlickable.top
		anchors.bottom: connectionsFlickable.bottom
		secondSize: Style.marginM
		targetItem: connectionsFlickable
	}

	Flickable {
		id: connectionsFlickable
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		contentHeight: connectionsColumn.height + 2 * Style.marginXL
		clip: true

		Column {
			id: connectionsColumn
			width: Style.sizeHintXXL
			spacing: Style.marginXL

			Column {
				width: parent.width
				spacing: Style.marginXS

				BaseText {
					text: qsTr("Tenant Connections")
					font.pixelSize: Style.fontSizeXL
					font.bold: true
					color: Style.textColor
				}

				BaseText {
					width: parent.width
					wrapMode: Text.WordWrap
					text: qsTr("Discover and connect with other tenants. Send a connection request by identifier, share a one-time connect code, or redeem a code you received. Accepting a request establishes an asymmetric relationship between both tenants.")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
			}

			Rectangle {
				width: parent.width
				height: 1
				color: Style.borderColor
			}

			// --- Create request / code form ---
			GroupElementView {
				id: createGroup
				width: parent.width
				visible: connectionsPage.__canManage

				TextInputElementView {
					id: targetIdentifierInput
					name: qsTr("Target Identifier")
					placeHolderText: qsTr("E-mail or slug of the tenant to invite")
				}

				ComboBoxElementView {
					id: sourceRoleInput
					name: qsTr("Proposed Source Role")
					currentIndex: 2
					model: roleModel
				}

				ComboBoxElementView {
					id: targetRoleInput
					name: qsTr("Proposed Target Role")
					currentIndex: 2
					model: roleModel
				}

				TextInputElementView {
					id: messageInput
					name: qsTr("Message")
					placeHolderText: qsTr("Optional message")
				}

				TextInputElementView {
					id: expiresInput
					name: qsTr("Expires At")
					placeHolderText: qsTr("Optional ISO timestamp — empty for no expiry")
				}

				Row {
					spacing: Style.marginM

					Button {
						text: qsTr("Send Request")
						onClicked: connectionsPage.__createRequest()
					}

					Button {
						text: qsTr("Create Connect Code")
						onClicked: connectionsPage.__createCode()
					}
				}
			}

			// --- Redeem code ---
			GroupElementView {
				id: redeemGroup
				width: parent.width
				visible: connectionsPage.__canManage

				TextInputElementView {
					id: redeemCodeInput
					name: qsTr("Redeem Connect Code")
					placeHolderText: qsTr("Paste a connect code to connect")
				}

				Button {
					text: qsTr("Redeem Code")
					onClicked: connectionsPage.__redeemCode()
				}
			}

			// --- Existing requests ---
			BaseText {
				text: qsTr("Connection Requests")
				font.pixelSize: Style.fontSizeL
				font.bold: true
				color: Style.textColor
			}

			BaseText {
				width: parent.width
				visible: !requestsList.count
				text: qsTr("No connection requests for this tenant.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			Column {
				id: requestsList
				width: parent.width
				spacing: Style.marginM

				property int count: connectionsPage.apiClient && connectionsPage.apiClient.connectionRequestsModel
					? connectionsPage.apiClient.connectionRequestsModel.count
					: 0

				Repeater {
					model: connectionsPage.apiClient ? connectionsPage.apiClient.connectionRequestsModel : null

					delegate: Rectangle {
						width: requestsList.width
						height: requestColumn.height + 2 * Style.marginM
						color: Style.alternateBaseColor
						radius: Style.radiusS
						border.color: Style.borderColor
						border.width: 1

						readonly property bool __isOutgoing: connectionsPage.tenantData
							&& model.sourceTenantId === connectionsPage.tenantData.m_id
						readonly property bool __isPending: (model.status || "") === "Pending"

						Column {
							id: requestColumn
							anchors.left: parent.left
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							anchors.leftMargin: Style.marginM
							anchors.rightMargin: Style.marginM
							spacing: Style.marginXS

							BaseText {
								width: parent.width
								elide: Text.ElideRight
								text: __isOutgoing
									? qsTr("To: %1").arg((model.targetIdentifier && model.targetIdentifier !== "") ? model.targetIdentifier : (model.targetTenantId || qsTr("(connect code)")))
									: qsTr("From: %1").arg(model.sourceTenantId || "")
								font.pixelSize: Style.fontSizeM
								color: Style.textColor
							}

							BaseText {
								width: parent.width
								elide: Text.ElideRight
								text: qsTr("Status: %1   Roles: %2 / %3")
									.arg(model.status || qsTr("Pending"))
									.arg(model.proposedSourceRole || qsTr("Partner"))
									.arg(model.proposedTargetRole || qsTr("Partner"))
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}

							BaseText {
								width: parent.width
								visible: model.connectCode && model.connectCode !== ""
								elide: Text.ElideRight
								text: qsTr("Connect code: %1").arg(model.connectCode || "")
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}

							BaseText {
								width: parent.width
								visible: model.message && model.message !== ""
								wrapMode: Text.WordWrap
								text: model.message || ""
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}

							Row {
								spacing: Style.marginM
								visible: connectionsPage.__canManage && __isPending

								Button {
									visible: !__isOutgoing
									text: qsTr("Accept")
									onClicked: {
										if (connectionsPage.apiClient)
											connectionsPage.apiClient.acceptConnectionRequest(
												model.requestId || "",
												connectionsPage.tenantData ? connectionsPage.tenantData.m_id : "")
									}
								}

								Button {
									visible: !__isOutgoing
									text: qsTr("Reject")
									onClicked: {
										if (connectionsPage.apiClient)
											connectionsPage.apiClient.rejectConnectionRequest(model.requestId || "")
									}
								}

								Button {
									visible: __isOutgoing
									text: qsTr("Revoke")
									onClicked: {
										if (connectionsPage.apiClient)
											connectionsPage.apiClient.revokeConnectionRequest(model.requestId || "")
									}
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
