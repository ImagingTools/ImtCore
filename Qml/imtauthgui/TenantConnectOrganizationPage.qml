// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantConnectOrganizationPage
 *
 * Allows the user to enter another organization's Connection Code to
 * send a connection request. Also displays incoming requests with
 * Approve/Reject actions.
 */
ViewBase {
	id: connectPage

	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor

	readonly property var tenantData: connectPage.model
	property var stateManager: null
	property var apiClient: null

	property bool sending: false
	property string lastError: ""

	function updateGui() {
		if (connectPage.apiClient && connectPage.tenantData
				&& connectPage.tenantData.m_id) {
			connectPage.apiClient.fetchConnectionRequests(connectPage.tenantData.m_id)
		}
	}

	Component.onCompleted: {
		connectPage.updateGui()
	}

	Connections {
		target: connectPage.apiClient

		function onConnectionRequestCreated(requestId) {
			connectPage.sending = false
			connectPage.lastError = ""
			codeInput.text = ""
			messageInput.text = ""
			PopupManager.addSuccessMessage(qsTr("Connection request sent successfully"), true)
		}

		function onConnectionRequestError(errorMessage) {
			connectPage.sending = false
			connectPage.lastError = errorMessage
		}

		function onConnectionRequestsReceived() {
			connectPage.refreshIncomingList()
		}

		function onConnectionRequestApproved(connectionId) {
			PopupManager.addSuccessMessage(qsTr("Connection request approved"), true)
			connectPage.updateGui()
		}

		function onConnectionRequestRejected() {
			PopupManager.addSuccessMessage(qsTr("Connection request rejected"), true)
			connectPage.updateGui()
		}
	}

	function refreshIncomingList() {
		var allRequests = connectPage.apiClient ? connectPage.apiClient.connectionRequestsModel : null
		var incoming = []
		if (allRequests && connectPage.tenantData) {
			for (var i = 0; i < allRequests.count; ++i) {
				var req = allRequests.get(i)
				if (req.targetTenantId === connectPage.tenantData.m_id
						&& req.status === "Pending") {
					incoming.push(req)
				}
			}
		}
		requestsList.model = incoming
	}

	Flickable {
		anchors.fill: parent
		anchors.topMargin: Style.marginXL
		contentHeight: mainColumn.height + Style.marginXL
		clip: true

		Column {
			id: mainColumn
			anchors.horizontalCenter: parent.horizontalCenter
			width: Math.min(parent.width - Style.marginXL * 2, 600)
			spacing: Style.marginXL

			Text {
				text: qsTr("Connect Organization")
				font.pixelSize: Style.fontSizeXL
				font.bold: true
				color: Style.textColor
			}

			Text {
				text: qsTr("Enter the Connection Code of the organization you want to connect with.")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
				wrapMode: Text.WordWrap
				width: parent.width
			}

			Rectangle {
				width: parent.width
				height: sendColumn.height + Style.marginL * 2
				color: Style.backgroundColor2
				radius: Style.radiusL
				border.width: 1
				border.color: Style.borderColor

				Column {
					id: sendColumn
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					anchors.margins: Style.marginL
					spacing: Style.marginM

					Column {
						width: parent.width
						spacing: Style.marginXS

						Text {
							text: qsTr("Connection Code")
							font.pixelSize: Style.fontSizeS
							color: Style.inactiveTextColor
						}

						CustomTextField {
							id: codeInput
							width: parent.width
							height: Style.controlHeightM
							textSize: Style.fontSizeM
							placeHolderText: qsTr("Enter code...")
						}
					}

					Column {
						width: parent.width
						spacing: Style.marginXS

						Text {
							text: qsTr("Message (optional)")
							font.pixelSize: Style.fontSizeS
							color: Style.inactiveTextColor
						}

						CustomTextField {
							id: messageInput
							width: parent.width
							height: Style.controlHeightM
							textSize: Style.fontSizeM
							placeHolderText: qsTr("Add a message...")
						}
					}

					Text {
						visible: connectPage.lastError !== ""
						text: connectPage.lastError
						font.pixelSize: Style.fontSizeS
						color: Style.errorColor
						wrapMode: Text.WordWrap
						width: parent.width
					}

					Button {
						text: connectPage.sending ? qsTr("Sending...") : qsTr("Send Request")
						enabled: codeInput.text.trim().length > 0 && !connectPage.sending
						onClicked: {
							if (connectPage.apiClient && connectPage.tenantData) {
								connectPage.sending = true
								connectPage.lastError = ""
								connectPage.apiClient.createConnectionRequest(
									connectPage.tenantData.m_id,
									codeInput.text.trim(),
									messageInput.text.trim())
							}
						}
					}
				}
			}

			Rectangle {
				width: parent.width
				height: 1
				color: Style.borderColor
			}

			Text {
				text: qsTr("Incoming Requests")
				font.pixelSize: Style.fontSizeL
				font.bold: true
				color: Style.textColor
			}

			Text {
				visible: requestsList.count === 0
				text: qsTr("No pending incoming requests.")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
			}

			ListView {
				id: requestsList
				width: parent.width
				height: requestsList.contentHeight
				interactive: false
				spacing: Style.marginS

				delegate: Rectangle {
					width: requestsList.width
					height: requestContent.height + Style.marginL * 2
					color: Style.backgroundColor2
					radius: Style.radiusL
					border.width: 1
					border.color: Style.borderColor

					Column {
						id: requestContent
						anchors.left: parent.left
						anchors.right: requestActions.left
						anchors.leftMargin: Style.marginL
						anchors.rightMargin: Style.marginS
						anchors.verticalCenter: parent.verticalCenter
						spacing: Style.marginXS

						Text {
							text: modelData.sourceTenantName ? modelData.sourceTenantName : (modelData.sourceTenantId ? modelData.sourceTenantId : "")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						Text {
							visible: modelData.message ? modelData.message !== "" : false
							text: modelData.message ? modelData.message : ""
							font.pixelSize: Style.fontSizeS
							color: Style.inactiveTextColor
						}

						Text {
							text: qsTr("Received: %1").arg(
								modelData.createdAt ? new Date(modelData.createdAt).toLocaleDateString() : "—")
							font.pixelSize: Style.fontSizeXS
							color: Style.inactiveTextColor
						}
					}

					Row {
						id: requestActions
						anchors.right: parent.right
						anchors.rightMargin: Style.marginL
						anchors.verticalCenter: parent.verticalCenter
						spacing: Style.marginS

						Button {
							text: qsTr("Approve")
							onClicked: {
								if (connectPage.apiClient && connectPage.tenantData) {
									connectPage.apiClient.approveConnectionRequest(
										modelData.id, connectPage.tenantData.m_id)
								}
							}
						}

						Button {
							text: qsTr("Reject")
							onClicked: {
								if (connectPage.apiClient && connectPage.tenantData) {
									connectPage.apiClient.rejectConnectionRequest(
										modelData.id, connectPage.tenantData.m_id)
								}
							}
						}
					}
				}
			}
		}
	}
}
