// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantIncomingRequestsPage
 *
 * Displays incoming connection requests with Approve/Reject actions.
 */
ViewBase {
	id: incomingPage

	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor

	readonly property var tenantData: incomingPage.model
	property var stateManager: null
	property var apiClient: null

	function updateGui() {
		if (incomingPage.apiClient && incomingPage.tenantData
				&& incomingPage.tenantData.m_id) {
			incomingPage.apiClient.fetchConnectionRequests(incomingPage.tenantData.m_id)
		}
	}

	Component.onCompleted: {
		updateGui()
	}

	Connections {
		target: incomingPage.apiClient

		function onConnectionRequestsReceived() {
			incomingPage.refreshList()
		}

		function onConnectionRequestApproved(connectionId) {
			PopupManager.addSuccessMessage(qsTr("Connection request approved"), true)
			incomingPage.updateGui()
		}

		function onConnectionRequestRejected() {
			PopupManager.addSuccessMessage(qsTr("Connection request rejected"), true)
			incomingPage.updateGui()
		}
	}

	function refreshList() {
		var allRequests = incomingPage.apiClient.connectionRequestsModel
		var incoming = []
		if (allRequests && incomingPage.tenantData) {
			for (var i = 0; i < allRequests.count; ++i) {
				var req = allRequests.get(i)
				if (req.targetTenantId === incomingPage.tenantData.m_id
						&& req.status === "Pending") {
					incoming.push(req)
				}
			}
		}
		requestsList.model = incoming
	}

	Column {
		anchors.top: parent.top
		anchors.topMargin: Style.marginL
		anchors.horizontalCenter: parent.horizontalCenter
		width: Math.min(parent.width - Style.marginXL * 2, 1000)
		spacing: Style.marginL

		Text {
			text: qsTr("Incoming Requests")
			font.pixelSize: Style.fontSizeXL
			font.bold: true
			color: Style.textColor
		}

		Text {
			text: qsTr("Pending connection requests from other organizations.")
			font.pixelSize: Style.fontSizeM
			color: Style.inactiveTextColor
			wrapMode: Text.WordWrap
			width: parent.width
		}

		ListView {
			id: requestsList
			width: parent.width
			height: incomingPage.height - y - Style.marginXL * 2
			clip: true
			spacing: Style.marginS

			delegate: Rectangle {
				width: requestsList.width
				height: requestRow.height + Style.marginM * 2
				color: Style.backgroundColor2
				radius: Style.radiusL

				Column {
					id: requestRow
					anchors.left: parent.left
					anchors.right: actionsRow.left
					anchors.margins: Style.marginL
					anchors.verticalCenter: parent.verticalCenter
					spacing: Style.marginXS

					Text {
						text: modelData.sourceTenantName ? modelData.sourceTenantName : (modelData.sourceTenantId ? modelData.sourceTenantId : "")
						font.pixelSize: Style.fontSizeL
						font.bold: true
						color: Style.textColor
					}

					Text {
						visible: modelData.message ? modelData.message !== "" : false
						text: modelData.message ? modelData.message : ""
						font.pixelSize: Style.fontSizeM
						color: Style.inactiveTextColor
					}

					Text {
						text: qsTr("Received: %1").arg(
							modelData.createdAt ? new Date(modelData.createdAt).toLocaleDateString() : "—")
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
					}
				}

				Row {
					id: actionsRow
					anchors.right: parent.right
					anchors.rightMargin: Style.marginL
					anchors.verticalCenter: parent.verticalCenter
					spacing: Style.marginS

					Button {
						text: qsTr("Approve")
						onClicked: {
							if (incomingPage.apiClient && incomingPage.tenantData) {
								incomingPage.apiClient.approveConnectionRequest(
									modelData.id, incomingPage.tenantData.m_id)
							}
						}
					}

					Button {
						text: qsTr("Reject")
						onClicked: {
							if (incomingPage.apiClient && incomingPage.tenantData) {
								incomingPage.apiClient.rejectConnectionRequest(
									modelData.id, incomingPage.tenantData.m_id)
							}
						}
					}
				}
			}

			Text {
				visible: requestsList.count === 0
				text: qsTr("No incoming connection requests.")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
				anchors.centerIn: parent
			}
		}
	}
}
