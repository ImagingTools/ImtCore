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
 * TenantConnectionOverviewView
 *
 * Overview/dashboard page for the Connections section.
 * Shows statistics cards, recent activity, and items requiring attention.
 */
ViewBase {
	id: overviewView

	contentColor: Style.baseColor

	readonly property var tenantData: overviewView.model
	property var stateManager: null
	property var apiClient: null

	readonly property bool __canManage: overviewView.stateManager
										&& (overviewView.stateManager.isCreator || overviewView.stateManager.isOwner)

	// --- Computed statistics ---
	readonly property int __incomingCount: overviewView.__countByDirection("incoming")
	readonly property int __outgoingCount: overviewView.__countByDirection("outgoing")
	readonly property int __pendingCount: overviewView.__countByStatus("Pending")
	readonly property int __activeCount: overviewView.__countByStatus("Accepted")

	function updateGui() {
		if (overviewView.apiClient && overviewView.tenantData && overviewView.tenantData.m_id) {
			overviewView.apiClient.fetchConnectionRequests(overviewView.tenantData.m_id)
		}
	}

	function updateModel() {
		// No model updates needed
	}

	Component.onCompleted: {
		if (overviewView.apiClient && overviewView.tenantData && overviewView.tenantData.m_id) {
			overviewView.apiClient.fetchConnectionRequests(overviewView.tenantData.m_id)
		}
	}

	onVisibleChanged: {
		if (overviewView.visible && overviewView.apiClient && overviewView.tenantData && overviewView.tenantData.m_id) {
			overviewView.apiClient.fetchConnectionRequests(overviewView.tenantData.m_id)
		}
	}

	// --- Real-time updates ---
	Connections {
		target: overviewView.apiClient

		function onConnectionRequestsReceived(forTenantId, requests) {
			if (!overviewView.tenantData || forTenantId !== overviewView.tenantData.m_id)
				return
			overviewView.__rebuildRecentActivity()
		}

		function onConnectionRequestCreated(requestId) {
			overviewView.__refreshData()
		}

		function onConnectionRequestAccepted(requestId) {
			overviewView.__refreshData()
		}

		function onConnectionRequestRejected(requestId) {
			overviewView.__refreshData()
		}

		function onConnectionRequestRevoked(requestId) {
			overviewView.__refreshData()
		}

		function onSubscriptionCrossTenantMessageReceived(notification) {
			if (overviewView.visible) {
				overviewView.__refreshData()
			}
		}

		function onSubscriptionCrossTenantMessageStatusChanged(notification) {
			if (overviewView.visible) {
				overviewView.__refreshData()
			}
		}
	}

	function __refreshData() {
		if (overviewView.apiClient && overviewView.tenantData && overviewView.tenantData.m_id) {
			overviewView.apiClient.fetchConnectionRequests(overviewView.tenantData.m_id)
		}
	}

	function __countByDirection(direction) {
		if (!overviewView.apiClient || !overviewView.apiClient.connectionRequestsModel || !overviewView.tenantData) {
			return 0
		}
		var mdl = overviewView.apiClient.connectionRequestsModel
		var count = 0
		for (var i = 0; i < mdl.count; i++) {
			var req = mdl.get(i)
			if (direction === "incoming" && req.targetTenantId === overviewView.tenantData.m_id) {
				count++
			} else if (direction === "outgoing" && req.sourceTenantId === overviewView.tenantData.m_id) {
				count++
			}
		}
		return count
	}

	function __countByStatus(status) {
		if (!overviewView.apiClient || !overviewView.apiClient.connectionRequestsModel) {
			return 0
		}
		var mdl = overviewView.apiClient.connectionRequestsModel
		var count = 0
		for (var i = 0; i < mdl.count; i++) {
			var req = mdl.get(i)
			if (req.status === status) {
				count++
			}
		}
		return count
	}

	function __rebuildRecentActivity() {
		recentActivityModel.clear()
		if (!overviewView.apiClient || !overviewView.apiClient.connectionRequestsModel) {
			return
		}
		var mdl = overviewView.apiClient.connectionRequestsModel
		var items = []
		for (var i = 0; i < mdl.count; i++) {
			var req = mdl.get(i)
			var desc = ""
			if (req.status === "Accepted") {
				desc = qsTr("Connection accepted — %1").arg(req.targetIdentifier || req.targetTenantId || req.sourceTenantId || "")
			} else if (req.status === "Rejected") {
				desc = qsTr("Request rejected — %1").arg(req.targetIdentifier || req.targetTenantId || req.sourceTenantId || "")
			} else if (req.status === "Revoked") {
				desc = qsTr("Request revoked — %1").arg(req.targetIdentifier || req.targetTenantId || req.sourceTenantId || "")
			} else if (req.status === "Pending") {
				if (req.sourceTenantId === (overviewView.tenantData ? overviewView.tenantData.m_id : "")) {
					desc = qsTr("Request sent to %1").arg(req.targetIdentifier || req.targetTenantId || "")
				} else {
					desc = qsTr("Request received from %1").arg(req.sourceTenantId || "")
				}
			} else {
				desc = qsTr("Connection event — %1").arg(req.status || "")
			}
			items.push({
				"description": desc,
				"timestamp": req.respondedAt || req.createdAt || "",
				"status": req.status || ""
			})
		}

		// Sort by timestamp descending and take max 10
		items.sort(function(a, b) {
			if (a.timestamp > b.timestamp) {
				return -1
			}
			if (a.timestamp < b.timestamp) {
				return 1
			}
			return 0
		})
		var max = Math.min(items.length, 10)
		for (var j = 0; j < max; j++) {
			recentActivityModel.append(items[j])
		}
	}

	function __getPendingIncomingRequests() {
		var result = []
		if (!overviewView.apiClient || !overviewView.apiClient.connectionRequestsModel || !overviewView.tenantData) {
			return result
		}
		var mdl = overviewView.apiClient.connectionRequestsModel
		for (var i = 0; i < mdl.count; i++) {
			var req = mdl.get(i)
			if (req.targetTenantId === overviewView.tenantData.m_id && req.status === "Pending") {
				result.push(req)
			}
		}
		return result
	}

	ListModel {
		id: recentActivityModel
	}

	ListModel {
		id: attentionModel
	}

	CustomScrollbar {
		id: scrollbar
		z: overviewView.z + 1
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
			// SECTION: Statistics
			// =============================================================
			GroupHeaderView {
				width: mainColumn.width
				title: qsTr("Statistics")
			}

			Row {
				id: statsRow
				width: mainColumn.width
				spacing: Style.marginM

				// --- Stat Card: Incoming Requests ---
				Rectangle {
					id: incomingCard
					width: (mainColumn.width - 3 * Style.marginM) / 4
					height: Style.controlHeightXL + Style.marginXL
					radius: Style.radiusS
					color: Style.alternateBaseColor
					border.color: Style.borderColor
					border.width: 1

					Column {
						anchors.centerIn: parent
						spacing: Style.marginXS

						BaseText {
							anchors.horizontalCenter: parent.horizontalCenter
							text: String(overviewView.__incomingCount)
							font.pixelSize: Style.fontSizeXL
							font.bold: true
							color: Style.textColor
						}

						BaseText {
							anchors.horizontalCenter: parent.horizontalCenter
							text: qsTr("Incoming")
							font.pixelSize: Style.fontSizeS
							color: Style.inactiveTextColor
						}
					}
				}

				// --- Stat Card: Outgoing Requests ---
				Rectangle {
					id: outgoingCard
					width: (mainColumn.width - 3 * Style.marginM) / 4
					height: Style.controlHeightXL + Style.marginXL
					radius: Style.radiusS
					color: Style.alternateBaseColor
					border.color: Style.borderColor
					border.width: 1

					Column {
						anchors.centerIn: parent
						spacing: Style.marginXS

						BaseText {
							anchors.horizontalCenter: parent.horizontalCenter
							text: String(overviewView.__outgoingCount)
							font.pixelSize: Style.fontSizeXL
							font.bold: true
							color: Style.textColor
						}

						BaseText {
							anchors.horizontalCenter: parent.horizontalCenter
							text: qsTr("Outgoing")
							font.pixelSize: Style.fontSizeS
							color: Style.inactiveTextColor
						}
					}
				}

				// --- Stat Card: Active Connections ---
				Rectangle {
					id: activeCard
					width: (mainColumn.width - 3 * Style.marginM) / 4
					height: Style.controlHeightXL + Style.marginXL
					radius: Style.radiusS
					color: Style.alternateBaseColor
					border.color: Style.borderColor
					border.width: 1

					Column {
						anchors.centerIn: parent
						spacing: Style.marginXS

						BaseText {
							anchors.horizontalCenter: parent.horizontalCenter
							text: String(overviewView.__activeCount)
							font.pixelSize: Style.fontSizeXL
							font.bold: true
							color: Style.textColor
						}

						BaseText {
							anchors.horizontalCenter: parent.horizontalCenter
							text: qsTr("Active")
							font.pixelSize: Style.fontSizeS
							color: Style.inactiveTextColor
						}
					}
				}

				// --- Stat Card: Pending Requests ---
				Rectangle {
					id: pendingCard
					width: (mainColumn.width - 3 * Style.marginM) / 4
					height: Style.controlHeightXL + Style.marginXL
					radius: Style.radiusS
					color: Style.alternateBaseColor
					border.color: Style.borderColor
					border.width: 1

					Column {
						anchors.centerIn: parent
						spacing: Style.marginXS

						BaseText {
							anchors.horizontalCenter: parent.horizontalCenter
							text: String(overviewView.__pendingCount)
							font.pixelSize: Style.fontSizeXL
							font.bold: true
							color: Style.textColor
						}

						BaseText {
							anchors.horizontalCenter: parent.horizontalCenter
							text: qsTr("Pending")
							font.pixelSize: Style.fontSizeS
							color: Style.inactiveTextColor
						}
					}
				}
			}

			// =============================================================
			// SECTION: Recent Activity
			// =============================================================
			GroupHeaderView {
				width: mainColumn.width
				title: qsTr("Recent Activity")
			}

			BaseText {
				width: mainColumn.width
				visible: recentActivityModel.count === 0
				text: qsTr("No recent activity.")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
			}

			Column {
				width: mainColumn.width
				spacing: Style.marginS

				Repeater {
					id: activityRepeater
					model: recentActivityModel

					delegate: Rectangle {
						id: activityDelegate
						width: mainColumn.width
						height: activityRow.height + 2 * Style.marginS
						radius: Style.radiusS
						color: "transparent"
						border.color: Style.borderColor
						border.width: 1

						readonly property var __item: model

						Row {
							id: activityRow
							anchors.left: activityDelegate.left
							anchors.right: activityDelegate.right
							anchors.verticalCenter: activityDelegate.verticalCenter
							anchors.margins: Style.marginM
							spacing: Style.marginM

							Column {
								width: activityRow.width - timestampText.width - Style.marginM
								spacing: Style.marginXS

								BaseText {
									width: parent.width
									elide: Text.ElideRight
									text: activityDelegate.__item.description || ""
									font.pixelSize: Style.fontSizeM
									color: Style.textColor
								}
							}

							BaseText {
								id: timestampText
								anchors.verticalCenter: parent.verticalCenter
								text: activityDelegate.__item.timestamp || ""
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
						}
					}
				}
			}

			// =============================================================
			// SECTION: Attention Required
			// =============================================================
			GroupHeaderView {
				width: mainColumn.width
				title: qsTr("Attention Required")
			}

			BaseText {
				width: mainColumn.width
				visible: !attentionRepeater.model || attentionRepeater.count === 0
				text: qsTr("No items require your attention.")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
			}

			Column {
				width: mainColumn.width
				spacing: Style.marginM

				Repeater {
					id: attentionRepeater
					model: overviewView.__getPendingIncomingRequests()

					delegate: Rectangle {
						id: attentionDelegate
						width: mainColumn.width
						height: attentionContent.height + 2 * Style.marginM
						radius: Style.radiusS
						color: Style.alternateBaseColor
						border.color: Style.linkColor
						border.width: 1

						readonly property var __req: modelData

						Column {
							id: attentionContent
							anchors.left: attentionDelegate.left
							anchors.right: attentionDelegate.right
							anchors.top: attentionDelegate.top
							anchors.margins: Style.marginM
							spacing: Style.marginS

							BaseText {
								width: attentionContent.width
								elide: Text.ElideRight
								text: qsTr("Incoming request from %1").arg(attentionDelegate.__req.sourceTenantId || "")
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: Style.textColor
							}

							BaseText {
								width: attentionContent.width
								visible: attentionDelegate.__req.message && attentionDelegate.__req.message !== ""
								wrapMode: Text.WordWrap
								text: attentionDelegate.__req.message || ""
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}

							Row {
								spacing: Style.marginM
								visible: overviewView.__canManage

								Button {
									text: qsTr("Accept")
									onClicked: {
										if (overviewView.apiClient) {
											overviewView.apiClient.acceptConnectionRequest(
														attentionDelegate.__req.requestId || "",
														overviewView.tenantData ? overviewView.tenantData.m_id : "")
										}
									}
								}

								Button {
									text: qsTr("Reject")
									onClicked: {
										if (overviewView.apiClient) {
											overviewView.apiClient.rejectConnectionRequest(attentionDelegate.__req.requestId || "")
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
}
