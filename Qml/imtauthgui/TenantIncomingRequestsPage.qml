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
			refreshList()
		}

		function onConnectionRequestApproved(connectionId) {
			PopupManager.addSuccessMessage(qsTr("Connection request approved"), true)
			updateGui()
		}

		function onConnectionRequestRejected() {
			PopupManager.addSuccessMessage(qsTr("Connection request rejected"), true)
			updateGui()
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
		anchors.fill: parent
		anchors.margins: Style.marginL
		spacing: Style.marginL

		Text {
			text: qsTr("Incoming Requests")
			font.pixelSize: Style.fontSizeH2
			font.bold: true
			color: Style.textColor
		}

		Text {
			text: qsTr("Pending connection requests from other organizations.")
			font.pixelSize: Style.fontSizeDefault
			color: Style.textSecondaryColor
			wrapMode: Text.WordWrap
			width: parent.width
		}

		ListView {
			id: requestsList
			width: parent.width
			height: parent.height - y
			clip: true
			spacing: Style.marginS

			delegate: Rectangle {
				width: requestsList.width
				height: requestRow.height + Style.marginM * 2
				color: Style.panelColor
				radius: Style.radiusS

				Column {
					id: requestRow
					anchors.left: parent.left
					anchors.right: actionsRow.left
					anchors.margins: Style.marginM
					anchors.verticalCenter: parent.verticalCenter
					spacing: Style.marginXS

					Text {
						text: modelData.sourceTenantName || modelData.sourceTenantId || ""
						font.pixelSize: Style.fontSizeDefault
						font.bold: true
						color: Style.textColor
					}

					Text {
						visible: modelData.message && modelData.message !== ""
						text: modelData.message || ""
						font.pixelSize: Style.fontSizeSmall
						color: Style.textSecondaryColor
					}

					Text {
						text: qsTr("Received: %1").arg(
							modelData.createdAt ? new Date(modelData.createdAt).toLocaleDateString() : "—")
						font.pixelSize: Style.fontSizeSmall
						color: Style.textSecondaryColor
					}
				}

				Row {
					id: actionsRow
					anchors.right: parent.right
					anchors.rightMargin: Style.marginM
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
				font.pixelSize: Style.fontSizeDefault
				color: Style.textSecondaryColor
				anchors.centerIn: parent
			}
		}
	}
}
