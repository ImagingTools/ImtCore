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
			connectPage.apiClient.fetchConnections(connectPage.tenantData.m_id)
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
			connectPage.updateGui()
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

		function onConnectionRequestCanceled() {
			PopupManager.addSuccessMessage(qsTr("Connection request canceled"), true)
			connectPage.updateGui()
		}

		function onConnectionsReceived() {
			connectPage.refreshConnectionsList()
		}

		function onConnectionRemoved(connectionId) {
			PopupManager.addSuccessMessage(qsTr("Connection removed"), true)
			connectPage.updateGui()
		}
	}

	function refreshIncomingList() {
		var allRequests = connectPage.apiClient ? connectPage.apiClient.connectionRequestsModel : null
		var incoming = []
		var outgoing = []
		if (allRequests && connectPage.tenantData) {
			for (var i = 0; i < allRequests.count; ++i) {
				var req = allRequests.get(i)
				if (req.targetTenantId === connectPage.tenantData.m_id
						&& req.status === "Pending") {
					incoming.push(req)
				}
				if (req.sourceTenantId === connectPage.tenantData.m_id
						&& req.status === "Pending") {
					outgoing.push(req)
				}
			}
		}
		requestsList.model = incoming
		sentRequestsList.model = outgoing
	}

	function refreshConnectionsList() {
		var allConnections = connectPage.apiClient ? connectPage.apiClient.connectionsModel : null
		var connections = []
		if (allConnections) {
			for (var i = 0; i < allConnections.count; ++i) {
				var conn = allConnections.get(i)
				if (conn.status === "Active") {
					connections.push(conn)
				}
			}
		}
		connectionsList.model = connections
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
					height: incomingContent.height + Style.marginL * 2
					color: Style.backgroundColor2
					radius: Style.radiusL
					border.width: 1
					border.color: Style.borderColor

					Column {
						id: incomingContent
						anchors.left: parent.left
						anchors.right: incomingActions.left
						anchors.leftMargin: Style.marginL
						anchors.rightMargin: Style.marginS
						anchors.verticalCenter: parent.verticalCenter
						spacing: Style.marginXS

						Text {
							text: modelData.sourceTenantName ? modelData.sourceTenantName : qsTr("Unknown Organization")
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
						id: incomingActions
						anchors.right: parent.right
						anchors.rightMargin: Style.marginL
						anchors.verticalCenter: parent.verticalCenter
						spacing: Style.marginS

						Rectangle {
							width: approveBtnText.contentWidth + Style.marginL * 2
							height: Style.controlHeightS
							radius: Style.radiusM
							color: "#3FB950"

							Text {
								id: approveBtnText
								anchors.centerIn: parent
								text: qsTr("Approve")
								font.pixelSize: Style.fontSizeS
								color: "#FFFFFF"
							}

							MouseArea {
								anchors.fill: parent
								cursorShape: Qt.PointingHandCursor
								onClicked: {
									if (connectPage.apiClient && connectPage.tenantData) {
										connectPage.apiClient.approveConnectionRequest(
											modelData.id, connectPage.tenantData.m_id)
									}
								}
							}
						}

						Rectangle {
							width: rejectBtnText.contentWidth + Style.marginL * 2
							height: Style.controlHeightS
							radius: Style.radiusM
							color: "#DA3633"

							Text {
								id: rejectBtnText
								anchors.centerIn: parent
								text: qsTr("Reject")
								font.pixelSize: Style.fontSizeS
								color: "#FFFFFF"
							}

							MouseArea {
								anchors.fill: parent
								cursorShape: Qt.PointingHandCursor
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

			Rectangle {
				width: parent.width
				height: 1
				color: Style.borderColor
			}

			Text {
				text: qsTr("Sent Requests")
				font.pixelSize: Style.fontSizeL
				font.bold: true
				color: Style.textColor
			}

			Text {
				visible: sentRequestsList.count === 0
				text: qsTr("No pending sent requests.")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
			}

			ListView {
				id: sentRequestsList
				width: parent.width
				height: sentRequestsList.contentHeight
				interactive: false
				spacing: Style.marginS

				delegate: Rectangle {
					width: sentRequestsList.width
					height: sentContent.height + Style.marginL * 2
					color: Style.backgroundColor2
					radius: Style.radiusL
					border.width: 1
					border.color: Style.borderColor

					Column {
						id: sentContent
						anchors.left: parent.left
						anchors.right: sentActions.left
						anchors.leftMargin: Style.marginL
						anchors.rightMargin: Style.marginS
						anchors.verticalCenter: parent.verticalCenter
						spacing: Style.marginXS

						Text {
							text: modelData.targetTenantName ? modelData.targetTenantName : qsTr("Unknown Organization")
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
							text: qsTr("Sent: %1").arg(
								modelData.createdAt ? new Date(modelData.createdAt).toLocaleDateString() : "—")
							font.pixelSize: Style.fontSizeXS
							color: Style.inactiveTextColor
						}
					}

					Row {
						id: sentActions
						anchors.right: parent.right
						anchors.rightMargin: Style.marginL
						anchors.verticalCenter: parent.verticalCenter
						spacing: Style.marginS

						Rectangle {
							width: cancelBtnText.contentWidth + Style.marginL * 2
							height: Style.controlHeightS
							radius: Style.radiusM
							color: Style.inactiveTextColor

							Text {
								id: cancelBtnText
								anchors.centerIn: parent
								text: qsTr("Cancel")
								font.pixelSize: Style.fontSizeS
								color: "#FFFFFF"
							}

							MouseArea {
								anchors.fill: parent
								cursorShape: Qt.PointingHandCursor
								onClicked: {
									if (connectPage.apiClient && connectPage.tenantData) {
										connectPage.apiClient.cancelConnectionRequest(
											modelData.id, connectPage.tenantData.m_id)
									}
								}
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
				text: qsTr("Established Connections")
				font.pixelSize: Style.fontSizeL
				font.bold: true
				color: Style.textColor
			}

			Text {
				text: qsTr("Organizations you are connected with.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			Text {
				visible: connectionsList.count === 0
				text: qsTr("No established connections yet.")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
			}

			ListView {
				id: connectionsList
				width: parent.width
				height: connectionsList.contentHeight
				interactive: false
				spacing: Style.marginS

				delegate: Rectangle {
					width: connectionsList.width
					height: connContent.height + Style.marginL * 2
					color: Style.backgroundColor2
					radius: Style.radiusL
					border.width: 1
					border.color: Style.borderColor

					Column {
						id: connContent
						anchors.left: parent.left
						anchors.right: connActions.left
						anchors.leftMargin: Style.marginL
						anchors.rightMargin: Style.marginS
						anchors.verticalCenter: parent.verticalCenter
						spacing: Style.marginXS

						Text {
							text: modelData.partnerName ? modelData.partnerName : qsTr("Unknown Organization")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
						}

						Text {
							visible: modelData.partnerOwnerName ? modelData.partnerOwnerName !== "" : false
							text: qsTr("Owner: %1").arg(modelData.partnerOwnerName || "")
							font.pixelSize: Style.fontSizeS
							color: Style.inactiveTextColor
						}

						Text {
							text: qsTr("Connected since: %1").arg(
								modelData.createdAt ? new Date(modelData.createdAt).toLocaleDateString() : "—")
							font.pixelSize: Style.fontSizeXS
							color: Style.inactiveTextColor
						}
					}

					Row {
						id: connActions
						anchors.right: parent.right
						anchors.rightMargin: Style.marginL
						anchors.verticalCenter: parent.verticalCenter
						spacing: Style.marginS

						Rectangle {
							width: removeBtnText.contentWidth + Style.marginL * 2
							height: Style.controlHeightS
							radius: Style.radiusM
							color: "#DA3633"

							Text {
								id: removeBtnText
								anchors.centerIn: parent
								text: qsTr("Remove")
								font.pixelSize: Style.fontSizeS
								color: "#FFFFFF"
							}

							MouseArea {
								anchors.fill: parent
								cursorShape: Qt.PointingHandCursor
								onClicked: {
									if (connectPage.apiClient && connectPage.tenantData) {
										connectPage.__connectionToRemoveId = modelData.id
										connectPage.__connectionToRemoveName = modelData.partnerName || qsTr("Unknown Organization")
										ModalDialogManager.openDialog(removeConnectionConfirmDialogComp)
									}
								}
							}
						}
					}
				}
			}
		}
	}

	property string __connectionToRemoveId: ""
	property string __connectionToRemoveName: ""

	Component {
		id: removeConnectionConfirmDialogComp
		MessageDialog {
			width: Style.sizeHintM
			title: qsTr("Remove connection")
			message: qsTr("Are you sure you want to remove the connection with \"%1\"?\n\nAll relationships with this organization will also be removed.").arg(connectPage.__connectionToRemoveName)
			onFinished: {
				if (buttonId == Enums.yes) {
					connectPage.apiClient.removeConnection(
						connectPage.__connectionToRemoveId, connectPage.tenantData.m_id)
				}
			}
		}
	}
}
