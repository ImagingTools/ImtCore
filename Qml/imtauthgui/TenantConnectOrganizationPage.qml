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

	function __date(value) {
		return value ? new Date(value).toLocaleDateString() : "—"
	}

	function __requestRow(request, tenantName) {
		var row = {}
		row.id = request.id || ""
		row.organization = tenantName ? tenantName : qsTr("Unknown Organization")
		row.message = request.message || ""
		row.date = connectPage.__date(request.createdAt)
		return row
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
					incoming.push(connectPage.__requestRow(req, req.sourceTenantName))
				}
				if (req.sourceTenantId === connectPage.tenantData.m_id
						&& req.status === "Pending") {
					outgoing.push(connectPage.__requestRow(req, req.targetTenantName))
				}
			}
		}
		requestsTable.elements = incoming
		sentRequestsTable.elements = outgoing

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
					var row = {}
					row.id = conn.id || ""
					row.organization = conn.partnerName ? conn.partnerName : qsTr("Unknown Organization")
					row.owner = conn.partnerOwnerName || ""
					row.date = connectPage.__date(conn.createdAt)
					connections.push(row)
				}
			}
		}
		connectionsTable.elements = connections
	}

	Flickable {
		anchors.fill: parent
		anchors.topMargin: Style.marginXL
		contentHeight: mainColumn.height + Style.marginXL
		clip: true

		Column {
			id: mainColumn
			anchors.horizontalCenter: parent.horizontalCenter
			width: Math.min(parent.width - Style.marginXL * 2, Style.contentWidthMax)
			spacing: Style.marginM

			// Width of the label column of the form rows, as in the document editors
			readonly property int labelWidth: Math.min(Style.sizeHintS, Math.round(mainColumn.width * 0.35))

			GroupHeaderView {
				width: parent.width
				title: qsTr("Connect Organization")
			}

			Text {
				text: qsTr("Enter the Connection Code of the organization you want to connect with.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
				wrapMode: Text.WordWrap
				width: parent.width
			}

			Rectangle {
				width: parent.width
				height: sendColumn.height + Style.marginL * 2
				color: Style.baseColor
				radius: Style.radiusM
				border.width: 1
				border.color: Style.borderColor

				Column {
					id: sendColumn
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					anchors.margins: Style.marginL
					spacing: Style.marginM

					Item {
						width: parent.width
						height: Style.controlHeightM

						Text {
							anchors.left: parent.left
							anchors.verticalCenter: parent.verticalCenter
							width: mainColumn.labelWidth
							text: qsTr("Connection Code")
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
							elide: Text.ElideRight
						}

						CustomTextField {
							id: codeInput
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							width: parent.width - mainColumn.labelWidth - Style.marginL
							height: Style.controlHeightM
							textSize: Style.fontSizeM
							placeHolderText: qsTr("Enter code...")
						}
					}

					Rectangle {
						width: parent.width
						height: 1
						color: Style.borderColor
					}

					Item {
						width: parent.width
						height: Style.controlHeightM

						Text {
							anchors.left: parent.left
							anchors.verticalCenter: parent.verticalCenter
							width: mainColumn.labelWidth
							text: qsTr("Message (optional)")
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: Style.textColor
							elide: Text.ElideRight
						}

						CustomTextField {
							id: messageInput
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							width: parent.width - mainColumn.labelWidth - Style.marginL
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
						anchors.right: parent.right
						variant: "primary"
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

			Item {
				width: parent.width
				height: Style.marginM
			}

			GroupHeaderView {
				width: parent.width
				title: qsTr("Incoming Requests")
				visible: connectPage.__canViewConnections
			}

			Text {
				visible: requestsTable.elementsCount === 0
				text: qsTr("No pending incoming requests.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			Table {
				id: requestsTable
				visible: connectPage.__canViewConnections && requestsTable.elementsCount > 0
				width: parent.width
				height: requestsTable.contentHeight + requestsTable.headerHeight
				canMoveColumns: false
				selectable: false
			}

			Item {
				width: parent.width
				height: Style.marginM
			}

			GroupHeaderView {
				width: parent.width
				title: qsTr("Sent Requests")
				visible: connectPage.__canViewConnections
			}

			Text {
				visible: sentRequestsTable.elementsCount === 0
				text: qsTr("No pending sent requests.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			Table {
				id: sentRequestsTable
				visible: connectPage.__canViewConnections && sentRequestsTable.elementsCount > 0
				width: parent.width
				height: sentRequestsTable.contentHeight + sentRequestsTable.headerHeight
				canMoveColumns: false
				selectable: false
			}

			Item {
				width: parent.width
				height: Style.marginM
			}

			GroupHeaderView {
				width: parent.width
				title: qsTr("Established Connections")
				visible: connectPage.__canViewConnections
			}

			Text {
				text: qsTr("Organizations you are connected with.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
				visible: connectPage.__canViewConnections
			}

			Text {
				visible: connectionsTable.elementsCount === 0
				text: qsTr("No established connections yet.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			Table {
				id: connectionsTable
				visible: connectPage.__canViewConnections && connectionsTable.elementsCount > 0
				width: parent.width
				height: connectionsTable.contentHeight + connectionsTable.headerHeight
				canMoveColumns: false
				selectable: false
			}
		}
	}

	TableHeaders {
		id: incomingHeaders

		Component.onCompleted: {
			addHeader("organization", qsTr("Organization"), false, false)
			addHeader("message", qsTr("Message"), false, false)
			addHeader("date", qsTr("Received"), false, false)
			addHeader("state", qsTr("State"), false, false)
			addHeader("actions", "", false, false)
			requestsTable.headers = incomingHeaders
			requestsTable.setColumnContentById("state", incomingStateCellComp)
			requestsTable.setColumnContentById("actions", incomingActionsCellComp)
		}
	}

	TableHeaders {
		id: sentHeaders

		Component.onCompleted: {
			addHeader("organization", qsTr("Organization"), false, false)
			addHeader("message", qsTr("Message"), false, false)
			addHeader("date", qsTr("Sent"), false, false)
			addHeader("state", qsTr("State"), false, false)
			addHeader("actions", "", false, false)
			sentRequestsTable.headers = sentHeaders
			sentRequestsTable.setColumnContentById("state", sentStateCellComp)
			sentRequestsTable.setColumnContentById("actions", sentActionsCellComp)
		}
	}

	TableHeaders {
		id: connectionsHeaders

		Component.onCompleted: {
			addHeader("organization", qsTr("Organization"), false, false)
			addHeader("owner", qsTr("Owner"), false, false)
			addHeader("date", qsTr("Connected since"), false, false)
			addHeader("actions", "", false, false)
			connectionsTable.headers = connectionsHeaders
			connectionsTable.setColumnContentById("actions", connectionActionsCellComp)
		}
	}

	Component {
		id: incomingStateCellComp

		TableCellDelegateBase {
			id: incomingStateCell
			readonly property string requestId: incomingStateCell.getRowData() ? (incomingStateCell.getRowData().id || "") : ""

			Text {
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				text: incomingStateCell.requestId === connectPage.processingRequestId
					? (connectPage.processingRequestAction === "approve" ? qsTr("Approving...") : qsTr("Rejecting..."))
					: (incomingStateCell.requestId === connectPage.lastIncomingRequestId ? connectPage.lastIncomingRequestStatus : "")
				font.pixelSize: Style.fontSizeS
				color: incomingStateCell.requestId === connectPage.lastIncomingRequestId && connectPage.lastIncomingRequestStatusError
					? Style.errorColor : Style.inactiveTextColor
			}
		}
	}

	Component {
		id: incomingActionsCellComp

		TableCellDelegateBase {
			id: incomingActionsCell
			readonly property string requestId: incomingActionsCell.getRowData() ? (incomingActionsCell.getRowData().id || "") : ""

			Row {
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				spacing: Style.marginS

				Button {
					variant: "primary"
					text: qsTr("Approve")
					enabled: connectPage.processingRequestId === "" && connectPage.__canManageConnections
					onClicked: {
						if (connectPage.apiClient && connectPage.tenantData) {
							connectPage.processingRequestId = incomingActionsCell.requestId
							connectPage.processingRequestAction = "approve"
							connectPage.__setIncomingStatus("", "", false)
							connectPage.apiClient.approveConnectionRequest(incomingActionsCell.requestId, connectPage.tenantData.m_id)
						}
					}
				}

				Button {
					text: qsTr("Reject")
					enabled: connectPage.processingRequestId === "" && connectPage.__canManageConnections
					onClicked: {
						if (connectPage.apiClient && connectPage.tenantData) {
							connectPage.processingRequestId = incomingActionsCell.requestId
							connectPage.processingRequestAction = "reject"
							connectPage.__setIncomingStatus("", "", false)
							connectPage.apiClient.rejectConnectionRequest(incomingActionsCell.requestId, connectPage.tenantData.m_id)
						}
					}
				}
			}
		}
	}

	Component {
		id: sentStateCellComp

		TableCellDelegateBase {
			id: sentStateCell
			readonly property string requestId: sentStateCell.getRowData() ? (sentStateCell.getRowData().id || "") : ""

			Text {
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				text: sentStateCell.requestId === connectPage.processingRequestId
					? qsTr("Canceling...")
					: (sentStateCell.requestId === connectPage.lastSentRequestId ? connectPage.lastSentRequestStatus : "")
				font.pixelSize: Style.fontSizeS
				color: sentStateCell.requestId === connectPage.lastSentRequestId && connectPage.lastSentRequestStatusError
					? Style.errorColor : Style.inactiveTextColor
			}
		}
	}

	Component {
		id: sentActionsCellComp

		TableCellDelegateBase {
			id: sentActionsCell
			readonly property string requestId: sentActionsCell.getRowData() ? (sentActionsCell.getRowData().id || "") : ""

			Button {
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				text: qsTr("Cancel")
				enabled: connectPage.processingRequestId === "" && connectPage.__canConnectOrganization
				onClicked: {
					if (connectPage.apiClient && connectPage.tenantData) {
						connectPage.processingRequestId = sentActionsCell.requestId
						connectPage.processingRequestAction = "cancel"
						connectPage.__setSentStatus("", "", false)
						connectPage.apiClient.cancelConnectionRequest(sentActionsCell.requestId, connectPage.tenantData.m_id)
					}
				}
			}
		}
	}

	Component {
		id: connectionActionsCellComp

		TableCellDelegateBase {
			id: connectionActionsCell
			readonly property var rowData: connectionActionsCell.getRowData()

			Button {
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				text: qsTr("Remove")
				enabled: connectPage.__canManageConnections
				onClicked: {
					if (connectPage.apiClient && connectPage.tenantData && connectionActionsCell.rowData) {
						connectPage.__connectionToRemoveId = connectionActionsCell.rowData.id
						connectPage.__connectionToRemoveName = connectionActionsCell.rowData.organization
						ModalDialogManager.openDialog(removeConnectionConfirmDialogComp)
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
