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
 * TenantConnectionsView
 *
 * Page for viewing established (active) tenant connections.
 * Shows accepted connection requests as active connections with details.
 */
ViewBase {
	id: connectionsView

	contentColor: Style.baseColor

	readonly property var tenantData: connectionsView.model
	property var stateManager: null
	property var apiClient: null
	
	function updateGui() {
		if (connectionsView.apiClient && connectionsView.tenantData && connectionsView.tenantData.m_id) {
			connectionsView.apiClient.fetchConnectionRequests(connectionsView.tenantData.m_id)
		}
	}
	
	function updateModel() {
		// No model updates needed for this view
	}
	
	Component.onCompleted: {
		if (connectionsView.apiClient && connectionsView.tenantData && connectionsView.tenantData.m_id) {
			connectionsView.apiClient.fetchConnectionRequests(connectionsView.tenantData.m_id)
		}
	}
	
	onVisibleChanged: {
		if (connectionsView.visible && connectionsView.apiClient && connectionsView.tenantData && connectionsView.tenantData.m_id) {
			connectionsView.apiClient.fetchConnectionRequests(connectionsView.tenantData.m_id)
		}
	}
	
	// --- Real-time updates ---
	Connections {
		target: connectionsView.apiClient
		
		function onConnectionRequestAccepted(requestId) {
			connectionsView.__refreshData()
		}
		
		function onSubscriptionCrossTenantMessageReceived(notification) {
			if (connectionsView.visible) {
				connectionsView.__refreshData()
			}
		}
		
		function onSubscriptionCrossTenantMessageStatusChanged(notification) {
			if (connectionsView.visible) {
				connectionsView.__refreshData()
			}
		}
	}
	
	function __refreshData() {
		if (connectionsView.apiClient && connectionsView.tenantData && connectionsView.tenantData.m_id) {
			connectionsView.apiClient.fetchConnectionRequests(connectionsView.tenantData.m_id)
		}
	}
	
	function __getActiveConnections() {
		var result = []
		if (!connectionsView.apiClient || !connectionsView.apiClient.connectionRequestsModel) {
			return result
		}
		var mdl = connectionsView.apiClient.connectionRequestsModel
		for (var i = 0; i < mdl.count; i++) {
			var req = mdl.get(i)
			if (req.status === "Accepted") {
				result.push(req)
			}
		}
		return result
	}
	
	CustomScrollbar {
		id: scrollbar
		z: connectionsView.z + 1
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
			// SECTION: Active Connections
			// =============================================================
			GroupHeaderView {
				width: mainColumn.width
				title: qsTr("Active Connections")
			}
			
			BaseText {
				width: mainColumn.width
				visible: activeRepeater.count === 0
				text: qsTr("No active connections yet. Send or accept a connection request to establish your first connection.")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
				wrapMode: Text.WordWrap
			}
			
			Column {
				width: mainColumn.width
				spacing: Style.marginM
				
				Repeater {
					id: activeRepeater
					model: connectionsView.__getActiveConnections()
					
					delegate: Rectangle {
						id: connectionDelegate
						width: mainColumn.width
						height: connContent.height + 2 * Style.marginM
						radius: Style.radiusS
						color: Style.alternateBaseColor
						border.color: Style.borderColor
						border.width: 1
						
						readonly property var __conn: modelData
						readonly property bool __isOutgoing: connectionDelegate.__conn.sourceTenantId === (connectionsView.tenantData ? connectionsView.tenantData.m_id : "")
						readonly property string __partnerTenantId: connectionDelegate.__isOutgoing
																	? (connectionDelegate.__conn.targetTenantId || connectionDelegate.__conn.targetIdentifier || "")
																	: (connectionDelegate.__conn.sourceTenantId || "")
						
						Column {
							id: connContent
							anchors.left: connectionDelegate.left
							anchors.right: connectionDelegate.right
							anchors.top: connectionDelegate.top
							anchors.margins: Style.marginM
							spacing: Style.marginXS
							
							Row {
								width: connContent.width
								spacing: Style.marginM
								
								BaseText {
									width: parent.width - connStatusBadge.width - Style.marginM
									elide: Text.ElideRight
									text: connectionDelegate.__partnerTenantId
									font.pixelSize: Style.fontSizeM
									font.bold: true
									color: Style.textColor
								}
								
								StatusBadge {
									id: connStatusBadge
									text: qsTr("Active")
									badgeColor: Style.linkColor
									textColor: Style.baseColor
								}
							}
							
							BaseText {
								width: connContent.width
								elide: Text.ElideRight
								text: qsTr("Roles: %1 → %2")
								.arg(connectionDelegate.__conn.proposedSourceRole || "")
								.arg(connectionDelegate.__conn.proposedTargetRole || "")
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
							
							BaseText {
								width: connContent.width
								visible: connectionDelegate.__conn.respondedAt && connectionDelegate.__conn.respondedAt !== ""
								elide: Text.ElideRight
								text: qsTr("Connected since: %1").arg(connectionDelegate.__conn.respondedAt || "")
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
							
							BaseText {
								width: connContent.width
								elide: Text.ElideRight
								text: connectionDelegate.__isOutgoing ? qsTr("Direction: Outgoing") : qsTr("Direction: Incoming")
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
						}
					}
				}
			}
		}
	}
}
