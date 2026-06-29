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
	readonly property bool __canConnectOrganization: connectPage.stateManager ? connectPage.stateManager.canConnectOrganization : false
	readonly property bool __canManageConnections: connectPage.stateManager ? connectPage.stateManager.canManageOrganizationConnections : false
	readonly property bool __canViewConnections: connectPage.stateManager ? connectPage.stateManager.canViewOrganizationConnections : false

	property bool sending: false
	property string lastError: ""
	property string processingRequestId: ""
	property string processingRequestAction: ""
	property string lastIncomingRequestId: ""
	property string lastIncomingRequestStatus: ""
	property bool lastIncomingRequestStatusError: false
	property string lastSentRequestId: ""
	property string lastSentRequestStatus: ""
	property bool lastSentRequestStatusError: false

	function __setIncomingStatus(requestId, statusText, isError) {
		connectPage.lastIncomingRequestId = requestId || ""
		connectPage.lastIncomingRequestStatus = statusText || ""
		connectPage.lastIncomingRequestStatusError = isError === true
	}

	function __setSentStatus(requestId, statusText, isError) {
		connectPage.lastSentRequestId = requestId || ""
		connectPage.lastSentRequestStatus = statusText || ""
		connectPage.lastSentRequestStatusError = isError === true
	}

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
			if (connectPage.processingRequestId !== "") {
				if (connectPage.processingRequestAction === "approve")
					connectPage.__setIncomingStatus(connectPage.processingRequestId, errorMessage || qsTr("Approve failed"), true)
				else if (connectPage.processingRequestAction === "reject")
					connectPage.__setIncomingStatus(connectPage.processingRequestId, errorMessage || qsTr("Reject failed"), true)
				else if (connectPage.processingRequestAction === "cancel")
					connectPage.__setSentStatus(connectPage.processingRequestId, errorMessage || qsTr("Cancel failed"), true)
			}
			connectPage.processingRequestId = ""
			connectPage.processingRequestAction = ""
			connectPage.lastError = errorMessage
		}

		function onRequestFailed(message) {
			connectPage.sending = false
			if (connectPage.processingRequestId !== "") {
				if (connectPage.processingRequestAction === "approve")
					connectPage.__setIncomingStatus(connectPage.processingRequestId, message || qsTr("Approve failed"), true)
				else if (connectPage.processingRequestAction === "reject")
					connectPage.__setIncomingStatus(connectPage.processingRequestId, message || qsTr("Reject failed"), true)
				else if (connectPage.processingRequestAction === "cancel")
					connectPage.__setSentStatus(connectPage.processingRequestId, message || qsTr("Cancel failed"), true)
			}
			connectPage.processingRequestId = ""
			connectPage.processingRequestAction = ""
			connectPage.lastError = message || ""
			if (message && message !== "") {
				PopupManager.addErrorMessage(message, true)
			}
			connectPage.updateGui()
		}

		function onConnectionRequestsReceived() {
			connectPage.refreshIncomingList()
		}

		function onConnectionRequestApproved(connectionId) {
			connectPage.__setIncomingStatus(connectPage.processingRequestId, qsTr("Approved"), false)
			connectPage.processingRequestId = ""
			connectPage.processingRequestAction = ""
			PopupManager.addSuccessMessage(qsTr("Connection request approved"), true)
			connectPage.updateGui()
		}

		function onConnectionRequestRejected() {
			connectPage.__setIncomingStatus(connectPage.processingRequestId, qsTr("Rejected"), false)
			connectPage.processingRequestId = ""
			connectPage.processingRequestAction = ""
			PopupManager.addSuccessMessage(qsTr("Connection request rejected"), true)
			connectPage.updateGui()
		}

		function onConnectionRequestCanceled() {
			connectPage.__setSentStatus(connectPage.processingRequestId, qsTr("Canceled"), false)
			connectPage.processingRequestId = ""
			connectPage.processingRequestAction = ""
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

		if (connectPage.lastIncomingRequestId !== "") {
			var incomingStillVisible = false
			for (var k = 0; k < incoming.length; ++k) {
				if (incoming[k].id === connectPage.lastIncomingRequestId) {
					incomingStillVisible = true
					break
				}
			}
			if (!incomingStillVisible) {
				connectPage.__setIncomingStatus("", "", false)
			}
		}

		if (connectPage.lastSentRequestId !== "") {
			var sentStillVisible = false
			for (var m = 0; m < outgoing.length; ++m) {
				if (outgoing[m].id === connectPage.lastSentRequestId) {
					sentStillVisible = true
					break
				}
			}
			if (!sentStillVisible) {
				connectPage.__setSentStatus("", "", false)
			}
		}

		if (connectPage.processingRequestId !== "") {
			var stillPending = false
			for (var j = 0; j < incoming.length; ++j) {
				if (incoming[j].id === connectPage.processingRequestId) {
					stillPending = true
					break
				}
			}
			if (!stillPending) {
				for (var n = 0; n < outgoing.length; ++n) {
					if (outgoing[n].id === connectPage.processingRequestId) {
						stillPending = true
						break
					}
				}
			}
			if (!stillPending) {
				connectPage.processingRequestId = ""
				connectPage.processingRequestAction = ""
			}
		}
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
						enabled: codeInput.text.trim().length > 0 && !connectPage.sending && connectPage.__canConnectOrganization
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
				visible: connectPage.__canViewConnections
			}

			Text {
				visible: requestsList.count === 0
				text: qsTr("No pending incoming requests.")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
			}

			ListView {
				id: requestsList
				visible: connectPage.__canViewConnections
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

						Text {
							visible: modelData.id === connectPage.processingRequestId
									 || (modelData.id === connectPage.lastIncomingRequestId && connectPage.lastIncomingRequestStatus !== "")
							text: modelData.id === connectPage.processingRequestId
								? (connectPage.processingRequestAction === "approve" ? qsTr("Approving...") : qsTr("Rejecting..."))
								: connectPage.lastIncomingRequestStatus
							font.pixelSize: Style.fontSizeXS
							color: (modelData.id === connectPage.lastIncomingRequestId && connectPage.lastIncomingRequestStatusError)
								? Style.errorColor : Style.inactiveTextColor
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
							color: connectPage.processingRequestId === modelData.id ? Style.inactiveTextColor : Style.successColor

							Text {
								id: approveBtnText
								anchors.centerIn: parent
								text: connectPage.processingRequestId === modelData.id ? qsTr("Approving...") : qsTr("Approve")
								font.pixelSize: Style.fontSizeS
								color: Style.baseColor
							}

							MouseArea {
								anchors.fill: parent
								cursorShape: Qt.PointingHandCursor
								enabled: connectPage.processingRequestId === "" && connectPage.__canManageConnections
								onClicked: {
									if (connectPage.apiClient && connectPage.tenantData) {
										connectPage.processingRequestId = modelData.id || ""
										connectPage.processingRequestAction = "approve"
										connectPage.__setIncomingStatus("", "", false)
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
							color: connectPage.processingRequestId === modelData.id ? Style.inactiveTextColor : Style.dangerColor

							Text {
								id: rejectBtnText
								anchors.centerIn: parent
								text: connectPage.processingRequestId === modelData.id ? qsTr("Rejecting...") : qsTr("Reject")
								font.pixelSize: Style.fontSizeS
								color: Style.baseColor
							}

							MouseArea {
								anchors.fill: parent
								cursorShape: Qt.PointingHandCursor
								enabled: connectPage.processingRequestId === "" && connectPage.__canManageConnections
								onClicked: {
									if (connectPage.apiClient && connectPage.tenantData) {
										connectPage.processingRequestId = modelData.id || ""
										connectPage.processingRequestAction = "reject"
										connectPage.__setIncomingStatus("", "", false)
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
				visible: connectPage.__canViewConnections
			}

			Text {
				visible: sentRequestsList.count === 0
				text: qsTr("No pending sent requests.")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
			}

			ListView {
				id: sentRequestsList
				visible: connectPage.__canViewConnections
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

						Text {
							visible: modelData.id === connectPage.processingRequestId
									 || (modelData.id === connectPage.lastSentRequestId && connectPage.lastSentRequestStatus !== "")
							text: modelData.id === connectPage.processingRequestId
								? qsTr("Canceling...")
								: connectPage.lastSentRequestStatus
							font.pixelSize: Style.fontSizeXS
							color: (modelData.id === connectPage.lastSentRequestId && connectPage.lastSentRequestStatusError)
								? Style.errorColor : Style.inactiveTextColor
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
								color: Style.baseColor
							}

							MouseArea {
								anchors.fill: parent
								cursorShape: Qt.PointingHandCursor
								enabled: connectPage.processingRequestId === "" && connectPage.__canConnectOrganization
								onClicked: {
									if (connectPage.apiClient && connectPage.tenantData) {
										connectPage.processingRequestId = modelData.id || ""
										connectPage.processingRequestAction = "cancel"
										connectPage.__setSentStatus("", "", false)
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
				visible: connectPage.__canViewConnections
			}

			Text {
				text: qsTr("Organizations you are connected with.")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
				visible: connectPage.__canViewConnections
			}

			Text {
				visible: connectionsList.count === 0
				text: qsTr("No established connections yet.")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
			}

			ListView {
				id: connectionsList
				visible: connectPage.__canViewConnections
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
							color: Style.dangerColor

							Text {
								id: removeBtnText
								anchors.centerIn: parent
								text: qsTr("Remove")
								font.pixelSize: Style.fontSizeS
								color: Style.baseColor
							}

							MouseArea {
								anchors.fill: parent
								cursorShape: Qt.PointingHandCursor
								enabled: connectPage.__canManageConnections
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
