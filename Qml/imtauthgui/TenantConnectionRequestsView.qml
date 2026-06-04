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
 * TenantConnectionRequestsView
 *
 * Page for managing connection requests:
 *   - Create new connection requests (top card)
 *   - View incoming requests with Accept/Reject actions (separate section)
 *   - View outgoing requests with Revoke action (separate section)
 */
ViewBase {
	id: requestsView
	contentColor: Style.baseColor

	readonly property var tenantData: requestsView.model
	property var stateManager: null
	property var apiClient: null
	
	readonly property bool __canManage: requestsView.stateManager
										&& (requestsView.stateManager.isCreator || requestsView.stateManager.isOwner)
	
	function updateGui() {
		if (requestsView.apiClient && requestsView.tenantData && requestsView.tenantData.m_id) {
			requestsView.apiClient.fetchConnectionRequests(requestsView.tenantData.m_id)
		}
	}
	
	function updateModel() {
		// No model updates needed for this view
	}
	
	Component.onCompleted: {
		if (requestsView.apiClient && requestsView.tenantData && requestsView.tenantData.m_id) {
			requestsView.apiClient.fetchConnectionRequests(requestsView.tenantData.m_id)
		}
	}
	
	onVisibleChanged: {
		if (requestsView.visible && requestsView.apiClient && requestsView.tenantData && requestsView.tenantData.m_id) {
			requestsView.apiClient.fetchConnectionRequests(requestsView.tenantData.m_id)
		}
	}
	
	// --- Real-time subscription-based updates ---
	Connections {
		target: requestsView.apiClient
		
		function onConnectionRequestCreated(requestId) {
			PopupManager.addSuccessMessage(qsTr("Connection request sent"), true)
			requestsView.__clearCreateForm()
			requestsView.__refreshList()
		}
		
		function onConnectionRequestAccepted(requestId) {
			PopupManager.addSuccessMessage(qsTr("Connection request accepted"), true)
			requestsView.__refreshList()
		}
		
		function onConnectionRequestRejected(requestId) {
			PopupManager.addSuccessMessage(qsTr("Connection request rejected"), true)
			requestsView.__refreshList()
		}
		
		function onConnectionRequestRevoked(requestId) {
			PopupManager.addSuccessMessage(qsTr("Connection request revoked"), true)
			requestsView.__refreshList()
		}
		
		// --- Subscription-driven real-time updates ---
		function onSubscriptionCrossTenantMessageReceived(notification) {
			if (requestsView.visible) {
				requestsView.__refreshList()
			}
		}
		
		function onSubscriptionCrossTenantMessageStatusChanged(notification) {
			if (requestsView.visible) {
				requestsView.__refreshList()
			}
		}
	}
	
	function __refreshList() {
		if (requestsView.apiClient && requestsView.tenantData && requestsView.tenantData.m_id) {
			requestsView.apiClient.fetchConnectionRequests(requestsView.tenantData.m_id)
		}
	}
	
	function __clearCreateForm() {
		targetIdentifierInput.text = ""
		connSourceRoleCB.currentIndex = 2
		connTargetRoleCB.currentIndex = 2
		connMessageInput.text = ""
	}
	
	// --- Filtered models ---
	function __getIncomingRequests() {
		var result = []
		if (!requestsView.apiClient || !requestsView.apiClient.connectionRequestsModel || !requestsView.tenantData) {
			return result
		}
		var mdl = requestsView.apiClient.connectionRequestsModel
		for (var i = 0; i < mdl.count; i++) {
			var req = mdl.get(i)
			if (req.targetTenantId === requestsView.tenantData.m_id) {
				result.push(req)
			}
		}
		return result
	}
	
	function __getOutgoingRequests() {
		var result = []
		if (!requestsView.apiClient || !requestsView.apiClient.connectionRequestsModel || !requestsView.tenantData) {
			return result
		}
		var mdl = requestsView.apiClient.connectionRequestsModel
		for (var i = 0; i < mdl.count; i++) {
			var req = mdl.get(i)
			if (req.sourceTenantId === requestsView.tenantData.m_id) {
				result.push(req)
			}
		}
		return result
	}
	
	CustomScrollbar {
		id: scrollbar
		z: requestsView.z + 1
		anchors.right: parent.right
		anchors.top: mainFlickable.top
		anchors.bottom: mainFlickable.bottom
		secondSize: Style.marginM
		targetItem: mainFlickable
	}
	
	Flickable {
		id: mainFlickable
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.horizontalCenter: parent.horizontalCenter
		width: Math.min(parent.width - Style.marginXL * 2, Style.sizeHintXXL)
		contentWidth: mainColumn.width
		contentHeight: mainColumn.height + 2 * Style.marginXL
		boundsBehavior: Flickable.StopAtBounds
		clip: true
		
		Column {
			id: mainColumn
			width: mainFlickable.width
			spacing: Style.marginXL
			
			// =============================================================
			// SECTION: Create Request (card)
			// =============================================================
			GroupHeaderView {
				width: mainColumn.width
				title: qsTr("Create Request")
			}
			
			Rectangle {
				id: createRequestCard
				width: mainColumn.width
				height: createRequestContent.height + 2 * Style.marginL
				radius: Style.radiusS
				color: Style.alternateBaseColor
				border.color: Style.borderColor
				border.width: 1
				
				Column {
					id: createRequestContent
					anchors.left: createRequestCard.left
					anchors.right: createRequestCard.right
					anchors.top: createRequestCard.top
					anchors.margins: Style.marginL
					spacing: Style.marginM
					
					GroupElementView {
						id: sendRequestGroup
						width: createRequestContent.width
						
						TextInputElementView {
							id: targetIdentifierInput
							name: qsTr("Target Identifier")
							placeHolderText: qsTr("E-mail or slug of the tenant to invite")
						}
						
						ComboBoxElementView {
							id: connSourceRoleCB
							name: qsTr("Source Role")
							model: connRoleModel
							currentIndex: 2
						}
						
						ComboBoxElementView {
							id: connTargetRoleCB
							name: qsTr("Target Role")
							model: connRoleModel
							currentIndex: 2
						}
						
						TextInputElementView {
							id: connMessageInput
							name: qsTr("Message")
							placeHolderText: qsTr("Optional message to include")
						}
						
						DateTimePickerElementView {
							id: connExpiresAtPicker
							name: qsTr("Expiration Date")
						}
					}
					
					Button {
						enabled: requestsView.__canManage
						text: qsTr("Send Request")
						onClicked: {
							var target = targetIdentifierInput.text.trim()
							if (target === "") {
								ModalDialogManager.showInfoDialog(qsTr("Target identifier is required."))
								return
							}
							var roleTokens = ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]
							var srcIdx = connSourceRoleCB.currentIndex >= 0 ? connSourceRoleCB.currentIndex : 2
							var tgtIdx = connTargetRoleCB.currentIndex >= 0 ? connTargetRoleCB.currentIndex : 2
							requestsView.apiClient.createConnectionRequest(
										requestsView.tenantData ? requestsView.tenantData.m_id : "",
										target,
										roleTokens[srcIdx],
										roleTokens[tgtIdx],
										connMessageInput.text.trim(),
										connExpiresAtPicker.getDateAsString())
						}
					}
				}
			}
			
			// =============================================================
			// SECTION: Incoming Requests
			// =============================================================
			GroupHeaderView {
				width: mainColumn.width
				title: qsTr("Incoming Requests")
			}
			
			BaseText {
				width: mainColumn.width
				visible: incomingRepeater.count === 0
				text: qsTr("No incoming connection requests.")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
			}
			
			Column {
				width: mainColumn.width
				spacing: Style.marginM
				
				Repeater {
					id: incomingRepeater
					model: requestsView.__getIncomingRequests()
					
					delegate: Rectangle {
						id: incomingRequestDelegate
						width: mainColumn.width
						height: inReqContent.height + 2 * Style.marginM
						color: Style.alternateBaseColor
						radius: Style.radiusS
						border.color: Style.borderColor
						border.width: 1
						
						readonly property var __req: modelData
						readonly property bool __isPending: (incomingRequestDelegate.__req.status || "") === "Pending"
						
						Column {
							id: inReqContent
							anchors.left: incomingRequestDelegate.left
							anchors.right: incomingRequestDelegate.right
							anchors.top: incomingRequestDelegate.top
							anchors.margins: Style.marginM
							spacing: Style.marginXS
							
							Row {
								width: inReqContent.width
								spacing: Style.marginM
								
								BaseText {
									elide: Text.ElideRight
									width: parent.width - inStatusBadge.width - Style.marginM
									text: qsTr("From: %1").arg(incomingRequestDelegate.__req.sourceTenantId || "")
									font.pixelSize: Style.fontSizeM
									font.bold: true
									color: Style.textColor
								}
								
								StatusBadge {
									id: inStatusBadge
									text: incomingRequestDelegate.__req.status || qsTr("Pending")
									badgeColor: incomingRequestDelegate.__isPending ? Style.errorColor : Style.baseColor
									textColor: incomingRequestDelegate.__isPending ? Style.baseColor : Style.textColor
								}
							}
							
							BaseText {
								width: inReqContent.width
								elide: Text.ElideRight
								text: qsTr("Roles: %1 → %2")
								.arg(incomingRequestDelegate.__req.proposedSourceRole || qsTr("Partner"))
								.arg(incomingRequestDelegate.__req.proposedTargetRole || qsTr("Partner"))
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
							
							BaseText {
								width: inReqContent.width
								visible: incomingRequestDelegate.__req.message && incomingRequestDelegate.__req.message !== ""
								wrapMode: Text.WordWrap
								text: incomingRequestDelegate.__req.message || ""
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
							
							BaseText {
								width: inReqContent.width
								visible: incomingRequestDelegate.__req.createdAt && incomingRequestDelegate.__req.createdAt !== ""
								elide: Text.ElideRight
								text: qsTr("Created: %1").arg(incomingRequestDelegate.__req.createdAt || "")
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
							
							// --- Action buttons (only for pending requests) ---
							Row {
								spacing: Style.marginM
								visible: requestsView.__canManage && incomingRequestDelegate.__isPending
								
								Button {
									text: qsTr("Accept")
									onClicked: {
										if (requestsView.apiClient) {
											requestsView.apiClient.acceptConnectionRequest(
														incomingRequestDelegate.__req.requestId || "",
														requestsView.tenantData ? requestsView.tenantData.m_id : "")
										}
									}
								}
								
								Button {
									text: qsTr("Reject")
									onClicked: {
										if (requestsView.apiClient) {
											requestsView.apiClient.rejectConnectionRequest(incomingRequestDelegate.__req.requestId || "")
										}
									}
								}
							}
						}
					}
				}
			}
			
			// =============================================================
			// SECTION: Outgoing Requests
			// =============================================================
			GroupHeaderView {
				width: mainColumn.width
				title: qsTr("Outgoing Requests")
			}
			
			BaseText {
				width: mainColumn.width
				visible: outgoingRepeater.count === 0
				text: qsTr("No outgoing connection requests.")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
			}
			
			Column {
				width: mainColumn.width
				spacing: Style.marginM
				
				Repeater {
					id: outgoingRepeater
					model: requestsView.__getOutgoingRequests()
					
					delegate: Rectangle {
						id: outgoingRequestDelegate
						width: mainColumn.width
						height: outReqContent.height + 2 * Style.marginM
						color: Style.alternateBaseColor
						radius: Style.radiusS
						border.color: Style.borderColor
						border.width: 1
						
						readonly property var __req: modelData
						readonly property bool __isPending: (outgoingRequestDelegate.__req.status || "") === "Pending"
						
						Column {
							id: outReqContent
							anchors.left: outgoingRequestDelegate.left
							anchors.right: outgoingRequestDelegate.right
							anchors.top: outgoingRequestDelegate.top
							anchors.margins: Style.marginM
							spacing: Style.marginXS
							
							Row {
								width: outReqContent.width
								spacing: Style.marginM
								
								BaseText {
									elide: Text.ElideRight
									width: parent.width - outStatusBadge.width - Style.marginM
									text: qsTr("To: %1").arg((outgoingRequestDelegate.__req.targetIdentifier && outgoingRequestDelegate.__req.targetIdentifier !== "")
																 ? outgoingRequestDelegate.__req.targetIdentifier
																 : (outgoingRequestDelegate.__req.targetTenantId || qsTr("(connect code)")))
									font.pixelSize: Style.fontSizeM
									font.bold: true
									color: Style.textColor
								}
								
								StatusBadge {
									id: outStatusBadge
									text: outgoingRequestDelegate.__req.status || qsTr("Pending")
									badgeColor: outgoingRequestDelegate.__isPending ? Style.errorColor : Style.baseColor
									textColor: outgoingRequestDelegate.__isPending ? Style.baseColor : Style.textColor
								}
							}
							
							BaseText {
								width: outReqContent.width
								elide: Text.ElideRight
								text: qsTr("Roles: %1 → %2")
								.arg(outgoingRequestDelegate.__req.proposedSourceRole || qsTr("Partner"))
								.arg(outgoingRequestDelegate.__req.proposedTargetRole || qsTr("Partner"))
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
							
							BaseText {
								width: outReqContent.width
								visible: outgoingRequestDelegate.__req.connectCode && outgoingRequestDelegate.__req.connectCode !== ""
								elide: Text.ElideRight
								text: qsTr("Connect code: %1").arg(outgoingRequestDelegate.__req.connectCode || "")
								font.pixelSize: Style.fontSizeS
								color: Style.linkColor
							}
							
							BaseText {
								width: outReqContent.width
								visible: outgoingRequestDelegate.__req.message && outgoingRequestDelegate.__req.message !== ""
								wrapMode: Text.WordWrap
								text: outgoingRequestDelegate.__req.message || ""
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
							
							BaseText {
								width: outReqContent.width
								visible: outgoingRequestDelegate.__req.createdAt && outgoingRequestDelegate.__req.createdAt !== ""
								elide: Text.ElideRight
								text: qsTr("Created: %1").arg(outgoingRequestDelegate.__req.createdAt || "")
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
							
							// --- Action buttons (only for pending requests) ---
							Row {
								spacing: Style.marginM
								visible: requestsView.__canManage && outgoingRequestDelegate.__isPending
								
								Button {
									text: qsTr("Revoke")
									onClicked: {
										if (requestsView.apiClient) {
											requestsView.apiClient.revokeConnectionRequest(outgoingRequestDelegate.__req.requestId || "")
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	TreeItemModel {
		id: connRoleModel
		Component.onCompleted: {
			var roles = ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]
			for (var i = 0; i < roles.length; i++) {
				var idx = connRoleModel.insertNewItem()
				connRoleModel.setData("id", roles[i], idx)
				connRoleModel.setData("name", roles[i], idx)
			}
		}
	}
}
