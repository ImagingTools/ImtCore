// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantPartnersPage
 *
 * The address book of the organization in one table: the requests waiting for an answer
 * and the connected organizations. A partner is invited by its code or found in the directory.
 * The roles and the terms of the cooperation are agreed in contracts, not here.
 */
ViewBase {
	id: partnersPage

	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor

	readonly property var tenantData: partnersPage.model
	property var stateManager: null
	property var apiClient: null
	// The directory of the organizations can be searched: the page offers to find a partner there
	property bool canFindInDirectory: false

	signal findInDirectoryRequested()

	readonly property string __tenantId: partnersPage.tenantData && partnersPage.tenantData.m_id ? String(partnersPage.tenantData.m_id) : ""
	readonly property bool __canConnect: partnersPage.stateManager ? partnersPage.stateManager.canConnectOrganization : false
	readonly property bool __canManage: partnersPage.stateManager ? partnersPage.stateManager.canManageOrganizationConnections : false

	// One answer at a time: the row being answered
	property string busyRowId: ""

	function updateGui() {
		if (!partnersPage.apiClient || partnersPage.__tenantId === "")
			return
		partnersPage.apiClient.fetchConnectionRequests(partnersPage.__tenantId)
		partnersPage.apiClient.fetchConnections(partnersPage.__tenantId)
	}

	Component.onCompleted: {
		partnersPage.updateGui()
	}

	// Notifications come in bursts and while the page is hidden: one reading when it is shown serves them all
	property bool __updatePending: false

	function __updateWhenVisible() {
		if (partnersPage.visible)
			updateTimer.restart()
		else
			partnersPage.__updatePending = true
	}

	onVisibleChanged: {
		if (partnersPage.visible && partnersPage.__updatePending) {
			partnersPage.__updatePending = false
			updateTimer.restart()
		}
	}

	Timer {
		id: updateTimer
		interval: 100
		onTriggered: {
			partnersPage.updateGui()
		}
	}

	// The rows are built once the reads of a refresh come in
	Timer {
		id: rebuildTimer
		interval: 50
		onTriggered: {
			partnersTable.elements = partnersPage.buildRows()
		}
	}

	function __done(message) {
		partnersPage.busyRowId = ""
		if (message !== "")
			PopupManager.addSuccessMessage(message, true)
		updateTimer.restart()
	}

	Connections {
		target: partnersPage.apiClient

		function onConnectionRequestsReceived() { rebuildTimer.restart() }
		function onConnectionsReceived() { rebuildTimer.restart() }

		function onConnectionRequestCreated(requestId) { partnersPage.__done(qsTr("The request was sent to the partner")) }
		function onConnectionRequestApproved(connectionId) { partnersPage.__done(qsTr("The partner was added to the address book")) }
		function onConnectionRequestRejected() { partnersPage.__done(qsTr("The request was declined")) }
		function onConnectionRequestCanceled() { partnersPage.__done(qsTr("The request was withdrawn")) }

		function onConnectionRemoved(connectionId) {
			// Keyed by the connection, as the notification of the same removal: one message for both
			PopupManager.addSuccessMessage(qsTr("The partner was removed from the address book"), true, "connection-removed-" + connectionId)
			partnersPage.__done("")
		}

		function onConnectionRequestError(errorMessage) {
			partnersPage.busyRowId = ""
			PopupManager.addErrorMessage(errorMessage, true)
		}

		function onRequestFailed(message) {
			partnersPage.busyRowId = ""
			if (message && message !== "")
				PopupManager.addErrorMessage(message, true)
			updateTimer.restart()
		}

		function onSubscriptionConnectionCodesChanged(notification) { partnersPage.__updateWhenVisible() }
		function onSubscriptionsResumed() { partnersPage.__updateWhenVisible() }
	}

	function __requestRow(request, isIncoming) {
		var row = {}
		row.id = "request:" + request.id
		row.requestId = request.id
		row.kind = isIncoming ? "incomingRequest" : "outgoingRequest"
		row.partner = (isIncoming ? request.sourceTenantName : request.targetTenantName) || qsTr("Unknown organization")
		row.since = request.createdAt || ""
		row.state = isIncoming ? qsTr("Waiting for your answer") : qsTr("Waiting for the partner")
		row.message = request.message || ""
		return row
	}

	function __partnerRow(connection) {
		var row = {}
		row.id = "connection:" + connection.id
		row.connectionId = connection.id
		row.partnerId = connection.partnerId
		row.partner = connection.partnerName || qsTr("Unknown organization")
		row.since = connection.createdAt || ""
		row.message = ""
		row.kind = "partner"
		row.state = qsTr("In the address book")
		return row
	}

	// The answers wanted from this organization come first
	function buildRows() {
		var rows = []
		if (!partnersPage.apiClient || partnersPage.__tenantId === "")
			return rows

		var requests = partnersPage.apiClient.connectionRequestsModel
		var connections = partnersPage.apiClient.connectionsModel
		var waiting = []
		var others = []

		if (requests) {
			for (var i = 0; i < requests.count; ++i) {
				var request = requests.get(i)
				if (request.status !== "Pending")
					continue
				if (request.targetTenantId === partnersPage.__tenantId)
					waiting.push(partnersPage.__requestRow(request, true))
				else if (request.sourceTenantId === partnersPage.__tenantId)
					others.push(partnersPage.__requestRow(request, false))
			}
		}

		if (connections) {
			for (var j = 0; j < connections.count; ++j) {
				var connection = connections.get(j)
				if (connection.status !== "Active")
					continue
				others.push(partnersPage.__partnerRow(connection))
			}
		}

		return waiting.concat(others)
	}

	function __rowById(rowId) {
		var rows = partnersTable.elements
		if (!rows)
			return null
		for (var i = 0; i < rows.length; ++i) {
			if (rows[i].id === rowId)
				return rows[i]
		}
		return null
	}

	function invitePartner() {
		ModalDialogManager.openDialog(inviteDialogComp, {})
	}

	function answer(rowId, accept) {
		var row = partnersPage.__rowById(rowId)
		if (!row || !partnersPage.apiClient || row.kind !== "incomingRequest")
			return
		partnersPage.busyRowId = rowId
		if (accept)
			partnersPage.apiClient.approveConnectionRequest(row.requestId, partnersPage.__tenantId)
		else
			partnersPage.apiClient.rejectConnectionRequest(row.requestId, partnersPage.__tenantId)
	}

	function withdraw(rowId) {
		var row = partnersPage.__rowById(rowId)
		if (!row || !partnersPage.apiClient || row.kind !== "outgoingRequest")
			return
		partnersPage.busyRowId = rowId
		partnersPage.apiClient.cancelConnectionRequest(row.requestId, partnersPage.__tenantId)
	}

	function removePartner(rowId) {
		var row = partnersPage.__rowById(rowId)
		if (!row)
			return
		ModalDialogManager.openDialog(removeDialogComp, {"rowId": rowId, "partnerName": row.partner})
	}

	CustomScrollbar {
		z: pageFlickable.z + 1
		anchors.right: parent.right
		anchors.top: pageFlickable.top
		anchors.bottom: pageFlickable.bottom
		secondSize: Style.marginM
		targetItem: pageFlickable
	}

	Flickable {
		id: pageFlickable

		anchors.fill: parent
		anchors.topMargin: Style.marginXL
		contentHeight: mainColumn.height + Style.marginXL
		clip: true

		Column {
			id: mainColumn
			anchors.horizontalCenter: parent.horizontalCenter
			width: Math.min(parent.width - Style.marginXL * 2, Style.contentWidthMax)
			spacing: Style.marginM

			Item {
				width: parent.width
				height: Math.max(titleColumn.height, headerButtons.height)

				Column {
					id: titleColumn
					anchors.left: parent.left
					anchors.right: headerButtons.left
					anchors.rightMargin: Style.marginL
					spacing: Style.marginXS

					GroupHeaderView {
						width: parent.width
						title: qsTr("Address book")
					}

					Text {
						width: parent.width
						text: qsTr("The address book of the organization. The terms of the cooperation with a partner are agreed in a contract.")
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
						wrapMode: Text.WordWrap
					}
				}

				Row {
					id: headerButtons
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					spacing: Style.marginS

					Button {
						text: qsTr("Find in the directory")
						visible: partnersPage.__canConnect && partnersPage.canFindInDirectory
						onClicked: {
							partnersPage.findInDirectoryRequested()
						}
					}

					Button {
						id: inviteButton
						variant: "primary"
						text: qsTr("Invite partner")
						visible: partnersPage.__canConnect
						onClicked: {
							partnersPage.invitePartner()
						}
					}
				}
			}

			Text {
				visible: partnersTable.elementsCount === 0
				text: qsTr("No partners yet. Invite one by the code the partner gave you or find one in the directory.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			Table {
				id: partnersTable
				visible: partnersTable.elementsCount > 0
				width: parent.width
				height: partnersTable.contentHeight + partnersTable.headerHeight
				canMoveColumns: true
				selectable: false
			}
		}
	}

	TableHeaders {
		id: partnersHeaders

		Component.onCompleted: {
			addHeader("partner", qsTr("Partner"), false, false)
			addHeader("message", qsTr("Message"), false, false)
			addHeader("state", qsTr("State"), false, false)
			addHeader("actions", "", false, false)
			partnersTable.headers = partnersHeaders
			partnersTable.setColumnContentById("actions", actionsCellComp)
		}
	}

	Component {
		id: actionsCellComp

		TableCellDelegateBase {
			id: actionsCell
			readonly property var rowData: actionsCell.getRowData()
			readonly property string rowId: actionsCell.rowData ? (actionsCell.rowData.id || "") : ""
			readonly property string kind: actionsCell.rowData ? (actionsCell.rowData.kind || "") : ""
			readonly property bool free: partnersPage.busyRowId === ""

			Row {
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				spacing: Style.marginS

				Button {
					variant: "primary"
					text: qsTr("Accept")
					visible: actionsCell.kind === "incomingRequest"
					enabled: actionsCell.free && partnersPage.__canManage
					onClicked: {
						partnersPage.answer(actionsCell.rowId, true)
					}
				}

				Button {
					text: qsTr("Decline")
					visible: actionsCell.kind === "incomingRequest"
					enabled: actionsCell.free && partnersPage.__canManage
					onClicked: {
						partnersPage.answer(actionsCell.rowId, false)
					}
				}

				Button {
					text: qsTr("Withdraw")
					visible: actionsCell.kind === "outgoingRequest"
					enabled: actionsCell.free && partnersPage.__canConnect
					onClicked: {
						partnersPage.withdraw(actionsCell.rowId)
					}
				}

				Button {
					text: qsTr("Remove")
					visible: actionsCell.kind === "partner"
					enabled: actionsCell.free && partnersPage.__canManage
					onClicked: {
						partnersPage.removePartner(actionsCell.rowId)
					}
				}
			}
		}
	}

	Component {
		id: inviteDialogComp

		PartnerInviteDialog {
			id: inviteDialog
			title: qsTr("Invite partner")
			onFinished: {
				if (buttonId == Enums.ok && partnersPage.apiClient && inviteDialog.contentItem) {
					partnersPage.busyRowId = "invite"
					partnersPage.apiClient.createConnectionRequest(partnersPage.__tenantId, inviteDialog.contentItem.code, inviteDialog.contentItem.message, "")
				}
			}
		}
	}

	Component {
		id: removeDialogComp

		MessageDialog {
			id: removeDialog
			property string rowId: ""
			property string partnerName: ""
			width: Style.sizeHintM
			title: qsTr("Remove partner")
			message: qsTr("Remove \"%1\" from the address book? The contracts with this organization stay in force.").arg(removeDialog.partnerName)
			onFinished: {
				var row = partnersPage.__rowById(removeDialog.rowId)
				if (buttonId == Enums.yes && row && partnersPage.apiClient) {
					partnersPage.busyRowId = removeDialog.rowId
					partnersPage.apiClient.removeConnection(row.connectionId, partnersPage.__tenantId)
				}
			}
		}
	}
}
