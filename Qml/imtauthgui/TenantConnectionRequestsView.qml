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
 *   - Create new connection requests
 *   - View incoming requests with Accept/Reject actions
 *   - View outgoing requests with Revoke action
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
			// SECTION: Send Connection Request by Email / Identifier
			// =============================================================
			GroupHeaderView {
				width: mainColumn.width
				title: qsTr("Send Connection Request")
			}
			
			GroupElementView {
				id: sendRequestGroup
				width: mainColumn.width
				
				TextInputElementView {
					id: targetIdentifierInput
					name: qsTr("Target Identifier")
					placeHolderText: qsTr("E-mail or slug of the tenant to invite")
				}
				
				ComboBoxElementView {
					id: connSourceRoleCB
					name: qsTr("Proposed Source Role")
					model: connRoleModel
					currentIndex: 2
				}
				
				ComboBoxElementView {
					id: connTargetRoleCB
					name: qsTr("Proposed Target Role")
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
					name: qsTr("Expires At")
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
			
			// =============================================================
			// SECTION: Connection Requests List (real-time)
			// =============================================================
			GroupHeaderView {
				width: mainColumn.width
				title: qsTr("Connection Requests")
			}
			
			// --- Incoming Requests Section ---
			GroupHeaderView {
				width: mainColumn.width
				title: qsTr("Incoming Requests")
			}
			
			BaseText {
				width: mainColumn.width
				visible: !incomingRepeater.model || incomingRepeater.model.count === 0
				text: qsTr("No incoming connection requests yet.")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
			}
			
			Column {
				width: mainColumn.width
				spacing: Style.marginM
				
				Repeater {
					id: incomingRepeater
					model: requestsView.apiClient ? requestsView.__getIncomingRequestsModel() : null
					
					delegate: Rectangle {
						id: incomingRequestDelegate
						width: mainColumn.width
						height: reqContent.height + 2 * Style.marginM
						color: Style.alternateBaseColor
						radius: Style.radiusS
						border.color: Style.borderColor
						border.width: 1
						
						readonly property var __req: model
						readonly property bool __isPending: (incomingRequestDelegate.__req.status || "") === "Pending"
						
						Column {
							id: reqContent
							anchors.left: incomingRequestDelegate.left
							anchors.right: incomingRequestDelegate.right
							anchors.top: incomingRequestDelegate.top
							anchors.margins: Style.marginM
							spacing: Style.marginXS
							
							BaseText {
								width: reqContent.width
								elide: Text.ElideRight
								text: qsTr("◀ From: %1").arg(incomingRequestDelegate.__req.sourceTenantId || "")
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: Style.textColor
							}
							
							BaseText {
								width: reqContent.width
								elide: Text.ElideRight
								text: qsTr("Status: %1   Roles: %2 / %3")
								.arg(incomingRequestDelegate.__req.status || qsTr("Pending"))
								.arg(incomingRequestDelegate.__req.proposedSourceRole || qsTr("Partner"))
								.arg(incomingRequestDelegate.__req.proposedTargetRole || qsTr("Partner"))
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
							
							BaseText {
								width: reqContent.width
								visible: incomingRequestDelegate.__req.message && incomingRequestDelegate.__req.message !== ""
								wrapMode: Text.WordWrap
								text: incomingRequestDelegate.__req.message || ""
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
							
							BaseText {
								width: reqContent.width
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
			
			// --- Outgoing Requests Section ---
			GroupHeaderView {
				width: mainColumn.width
				title: qsTr("Outgoing Requests")
			}
			
			BaseText {
				width: mainColumn.width
				visible: !outgoingRepeater.model || outgoingRepeater.model.count === 0
				text: qsTr("No outgoing connection requests yet.")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
			}
			
			Column {
				width: mainColumn.width
				spacing: Style.marginM
				
				Repeater {
					id: outgoingRepeater
					model: requestsView.apiClient ? requestsView.__getOutgoingRequestsModel() : null
					
					delegate: Rectangle {
						id: outgoingRequestDelegate
						width: mainColumn.width
						height: reqContent2.height + 2 * Style.marginM
						color: Style.alternateBaseColor
						radius: Style.radiusS
						border.color: Style.borderColor
						border.width: 1
						
						readonly property var __req: model
						readonly property bool __isPending: (outgoingRequestDelegate.__req.status || "") === "Pending"
						
						Column {
							id: reqContent2
							anchors.left: outgoingRequestDelegate.left
							anchors.right: outgoingRequestDelegate.right
							anchors.top: outgoingRequestDelegate.top
							anchors.margins: Style.marginM
							spacing: Style.marginXS
							
							BaseText {
								width: parent.width
								elide: Text.ElideRight
								text: qsTr("▶ To: %1").arg((outgoingRequestDelegate.__req.targetIdentifier && outgoingRequestDelegate.__req.targetIdentifier !== "")
															 ? outgoingRequestDelegate.__req.targetIdentifier
															 : (outgoingRequestDelegate.__req.targetTenantId || qsTr("(connect code)")))
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: Style.textColor
							}
							
							BaseText {
								width: parent.width
								elide: Text.ElideRight
								text: qsTr("Status: %1   Roles: %2 / %3")
								.arg(outgoingRequestDelegate.__req.status || qsTr("Pending"))
								.arg(outgoingRequestDelegate.__req.proposedSourceRole || qsTr("Partner"))
								.arg(outgoingRequestDelegate.__req.proposedTargetRole || qsTr("Partner"))
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
							
							BaseText {
								width: parent.width
								visible: outgoingRequestDelegate.__req.connectCode && outgoingRequestDelegate.__req.connectCode !== ""
								elide: Text.ElideRight
								text: qsTr("Connect code: %1").arg(outgoingRequestDelegate.__req.connectCode || "")
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
							
							BaseText {
								width: parent.width
								visible: outgoingRequestDelegate.__req.message && outgoingRequestDelegate.__req.message !== ""
								wrapMode: Text.WordWrap
								text: outgoingRequestDelegate.__req.message || ""
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
							
							BaseText {
								width: parent.width
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
	
	// --- Helper functions for filtering requests ---
	function __getIncomingRequestsModel() {
		if (!requestsView.apiClient || !requestsView.apiClient.connectionRequestsModel || !requestsView.tenantData) {
			return null
		}
		
		var model = requestsView.apiClient.connectionRequestsModel
		var filteredModel = []
		
		for (var i = 0; i < model.count; i++) {
			var req = model.get(i)
			// Incoming requests are those where the current tenant is the target
			if (req.targetTenantId === requestsView.tenantData.m_id) {
				filteredModel.push(req)
			}
		}
		
		return filteredModel
	}
	
	function __getOutgoingRequestsModel() {
		if (!requestsView.apiClient || !requestsView.apiClient.connectionRequestsModel || !requestsView.tenantData) {
			return null
		}
		
		var model = requestsView.apiClient.connectionRequestsModel
		var filteredModel = []
		
		for (var i = 0; i < model.count; i++) {
			var req = model.get(i)
			// Outgoing requests are those where the current tenant is the source
			if (req.sourceTenantId === requestsView.tenantData.m_id) {
				filteredModel.push(req)
			}
		}
		
		return filteredModel
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
