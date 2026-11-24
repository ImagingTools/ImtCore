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
 * The partners of the organization in one table: the requests waiting for an answer,
 * the connected organizations with the terms of the relationship, and the new terms
 * waiting for an answer. A partner is invited by its code; the request carries the
 * terms, so the partner accepts once.
 */
ViewBase {
	id: partnersPage

	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor

	readonly property var tenantData: partnersPage.model
	property var stateManager: null
	property var apiClient: null

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
		partnersPage.apiClient.fetchTenantRelationships(partnersPage.__tenantId)
		partnersPage.apiClient.fetchRelationshipProposals(partnersPage.__tenantId)
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
		function onTenantRelationshipsReceived() { rebuildTimer.restart() }
		function onRelationshipProposalsReceived() { rebuildTimer.restart() }

		function onConnectionRequestCreated(requestId) { partnersPage.__done(qsTr("The request was sent to the partner")) }
		function onConnectionRequestApproved(connectionId) { partnersPage.__done(qsTr("The partnership came into force")) }
		function onConnectionRequestRejected() { partnersPage.__done(qsTr("The request was declined")) }
		function onConnectionRequestCanceled() { partnersPage.__done(qsTr("The request was withdrawn")) }
		function onRelationshipProposalCreated() { partnersPage.__done(qsTr("The new terms were sent to the partner")) }
		function onRelationshipProposalApproved(relationshipId) { partnersPage.__done(qsTr("The new terms came into force")) }
		function onRelationshipProposalRejected() { partnersPage.__done(qsTr("The new terms were declined")) }
		function onRelationshipProposalCanceled() { partnersPage.__done(qsTr("The new terms were withdrawn")) }

		function onConnectionRemoved(connectionId) {
			// Keyed by the connection, as the notification of the same removal: one message for both
			PopupManager.addSuccessMessage(qsTr("The partnership was ended"), true, "connection-removed-" + connectionId)
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

	function roleName(role) {
		if (role === "Supplier")
			return qsTr("supplier")
		if (role === "Customer")
			return qsTr("customer")
		if (role === "Affiliate")
			return qsTr("affiliate")
		if (role === "Parent")
			return qsTr("parent organization")
		if (role === "Child")
			return qsTr("subsidiary")
		return qsTr("partner")
	}

	// The terms seen from this organization
	function termsText(myRole, partnerRole, scope) {
		var text = qsTr("We are %1, they are %2").arg(partnersPage.roleName(myRole)).arg(partnersPage.roleName(partnerRole))
		if (scope && scope !== "")
			text = text + " · " + scope
		return text
	}

	function __findActiveRelationship(relationships, connectionId) {
		for (var i = 0; i < relationships.count; ++i) {
			var rel = relationships.get(i)
			if (rel.connectionId === connectionId && rel.status === "Active")
				return rel
		}
		return null
	}

	// A proposal waits while only its initiator agreed
	function __findOpenProposal(proposals, connectionId) {
		for (var i = 0; i < proposals.count; ++i) {
			var proposal = proposals.get(i)
			if (proposal.connectionId === connectionId && proposal.status === "ApprovedByInitiator")
				return proposal
		}
		return null
	}

	function __requestRow(request, isIncoming) {
		var row = {}
		row.id = "request:" + request.id
		row.requestId = request.id
		row.kind = isIncoming ? "incomingRequest" : "outgoingRequest"
		row.partner = (isIncoming ? request.sourceTenantName : request.targetTenantName) || qsTr("Unknown organization")
		// The source role is the one of the requesting organization
		if (request.proposedSourceRole !== "") {
			row.terms = isIncoming
				? partnersPage.termsText(request.proposedTargetRole, request.proposedSourceRole, request.proposedScope)
				: partnersPage.termsText(request.proposedSourceRole, request.proposedTargetRole, request.proposedScope)
		}
		else {
			row.terms = qsTr("Connection without terms")
		}
		row.state = isIncoming ? qsTr("Waiting for your answer") : qsTr("Waiting for the partner")
		row.message = request.message || ""
		return row
	}

	function __partnerRow(connection, relationship, proposal) {
		var row = {}
		row.id = "connection:" + connection.id
		row.connectionId = connection.id
		row.partnerId = connection.partnerId
		row.partner = connection.partnerName || qsTr("Unknown organization")
		row.relationshipId = relationship ? relationship.id : ""
		var isSource = relationship ? relationship.sourceTenantId === partnersPage.__tenantId : true
		row.myRole = relationship ? (isSource ? relationship.sourceRole : relationship.targetRole) : "Partner"
		row.partnerRole = relationship ? (isSource ? relationship.targetRole : relationship.sourceRole) : "Partner"
		row.scope = relationship ? relationship.scope : ""
		row.terms = relationship ? partnersPage.termsText(row.myRole, row.partnerRole, row.scope) : qsTr("No terms yet")
		row.message = ""
		row.kind = "partner"
		row.state = qsTr("Active")
		if (proposal) {
			row.proposalId = proposal.id
			var isInitiator = proposal.initiatorTenantId === partnersPage.__tenantId
			row.kind = isInitiator ? "outgoingProposal" : "incomingProposal"
			row.state = isInitiator ? qsTr("New terms wait for the partner") : qsTr("New terms wait for your answer")
			// The roles of a proposal are the ones of the source and the target of its relationship, the initiator for a new one
			var proposalIsSource = relationship ? isSource : isInitiator
			row.terms = proposal.proposalType === "Delete"
				? qsTr("End of the partnership proposed")
				: partnersPage.termsText(proposalIsSource ? proposal.proposedSourceRole : proposal.proposedTargetRole,
										proposalIsSource ? proposal.proposedTargetRole : proposal.proposedSourceRole,
										proposal.proposedScope)
		}
		return row
	}

	// The answers wanted from this organization come first
	function buildRows() {
		var rows = []
		if (!partnersPage.apiClient || partnersPage.__tenantId === "")
			return rows

		var requests = partnersPage.apiClient.connectionRequestsModel
		var connections = partnersPage.apiClient.connectionsModel
		var relationships = partnersPage.apiClient.tenantRelationshipsModel
		var proposals = partnersPage.apiClient.relationshipProposalsModel
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

		if (connections && relationships && proposals) {
			for (var j = 0; j < connections.count; ++j) {
				var connection = connections.get(j)
				if (connection.status !== "Active")
					continue
				var row = partnersPage.__partnerRow(connection,
												partnersPage.__findActiveRelationship(relationships, connection.id),
												partnersPage.__findOpenProposal(proposals, connection.id))
				if (row.kind === "incomingProposal")
					waiting.push(row)
				else
					others.push(row)
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

	function proposeTerms(rowId) {
		var row = partnersPage.__rowById(rowId)
		if (!row)
			return
		ModalDialogManager.openDialog(termsDialogComp, {"rowId": rowId, "initialMyRole": row.myRole, "initialPartnerRole": row.partnerRole, "initialScope": row.scope})
	}

	function answer(rowId, accept) {
		var row = partnersPage.__rowById(rowId)
		if (!row || !partnersPage.apiClient)
			return
		partnersPage.busyRowId = rowId
		if (row.kind === "incomingRequest") {
			if (accept)
				partnersPage.apiClient.approveConnectionRequest(row.requestId, partnersPage.__tenantId)
			else
				partnersPage.apiClient.rejectConnectionRequest(row.requestId, partnersPage.__tenantId)
		}
		else if (row.kind === "incomingProposal") {
			if (accept)
				partnersPage.apiClient.approveRelationshipProposal(row.proposalId, partnersPage.__tenantId)
			else
				partnersPage.apiClient.rejectRelationshipProposal(row.proposalId, partnersPage.__tenantId)
		}
	}

	function withdraw(rowId) {
		var row = partnersPage.__rowById(rowId)
		if (!row || !partnersPage.apiClient)
			return
		partnersPage.busyRowId = rowId
		if (row.kind === "outgoingRequest")
			partnersPage.apiClient.cancelConnectionRequest(row.requestId, partnersPage.__tenantId)
		else if (row.kind === "outgoingProposal")
			partnersPage.apiClient.cancelRelationshipProposal(row.proposalId, partnersPage.__tenantId)
	}

	function endPartnership(rowId) {
		var row = partnersPage.__rowById(rowId)
		if (!row)
			return
		ModalDialogManager.openDialog(endDialogComp, {"rowId": rowId, "partnerName": row.partner})
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

			Item {
				width: parent.width
				height: Math.max(titleColumn.height, inviteButton.height)

				Column {
					id: titleColumn
					anchors.left: parent.left
					anchors.right: inviteButton.left
					anchors.rightMargin: Style.marginL
					spacing: Style.marginXS

					GroupHeaderView {
						width: parent.width
						title: qsTr("Partners")
					}

					Text {
						width: parent.width
						text: qsTr("Organizations you work with. A partnership is in force when both sides agree.")
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
						wrapMode: Text.WordWrap
					}
				}

				Button {
					id: inviteButton
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					variant: "primary"
					text: qsTr("Invite partner")
					visible: partnersPage.__canConnect
					onClicked: {
						partnersPage.invitePartner()
					}
				}
			}

			Text {
				visible: partnersTable.elementsCount === 0
				text: qsTr("No partners yet. Invite one by the code the partner gave you.")
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
			addHeader("terms", qsTr("Terms"), false, false)
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
					visible: actionsCell.kind === "incomingRequest" || actionsCell.kind === "incomingProposal"
					enabled: actionsCell.free && partnersPage.__canManage
					onClicked: {
						partnersPage.answer(actionsCell.rowId, true)
					}
				}

				Button {
					text: qsTr("Decline")
					visible: actionsCell.kind === "incomingRequest" || actionsCell.kind === "incomingProposal"
					enabled: actionsCell.free && partnersPage.__canManage
					onClicked: {
						partnersPage.answer(actionsCell.rowId, false)
					}
				}

				Button {
					text: qsTr("Withdraw")
					visible: actionsCell.kind === "outgoingRequest" || actionsCell.kind === "outgoingProposal"
					enabled: actionsCell.free && partnersPage.__canConnect
					onClicked: {
						partnersPage.withdraw(actionsCell.rowId)
					}
				}

				Button {
					text: qsTr("Change terms")
					visible: actionsCell.kind === "partner"
					enabled: actionsCell.free && partnersPage.__canConnect
					onClicked: {
						partnersPage.proposeTerms(actionsCell.rowId)
					}
				}

				Button {
					text: qsTr("End")
					visible: actionsCell.kind === "partner"
					enabled: actionsCell.free && partnersPage.__canManage
					onClicked: {
						partnersPage.endPartnership(actionsCell.rowId)
					}
				}
			}
		}
	}

	Component {
		id: inviteDialogComp

		PartnerTermsDialog {
			id: inviteDialog
			title: qsTr("Invite partner")
			showCode: true
			onFinished: {
				if (buttonId == Enums.ok && partnersPage.apiClient && inviteDialog.contentItem) {
					partnersPage.busyRowId = "invite"
					partnersPage.apiClient.createConnectionRequest(partnersPage.__tenantId, inviteDialog.contentItem.code, inviteDialog.contentItem.message,
																	inviteDialog.contentItem.myRole, inviteDialog.contentItem.partnerRole, inviteDialog.contentItem.scope)
				}
			}
		}
	}

	Component {
		id: termsDialogComp

		PartnerTermsDialog {
			id: termsDialog
			property string rowId: ""
			title: qsTr("Change terms")
			showCode: false
			onFinished: {
				var row = partnersPage.__rowById(termsDialog.rowId)
				var content = termsDialog.contentItem
				if (buttonId != Enums.ok || !row || !content || !partnersPage.apiClient)
					return
				// The roles go to the source and the target of the relationship; a new one has this organization as its source
				var relationships = partnersPage.apiClient.tenantRelationshipsModel
				var relationship = row.relationshipId !== "" ? partnersPage.__findActiveRelationship(relationships, row.connectionId) : null
				var isSource = relationship ? relationship.sourceTenantId === partnersPage.__tenantId : true
				partnersPage.busyRowId = termsDialog.rowId
				partnersPage.apiClient.createRelationshipProposal(
							partnersPage.__tenantId,
							row.partnerId,
							relationship ? "Update" : "Create",
							row.relationshipId,
							isSource ? content.myRole : content.partnerRole,
							isSource ? content.partnerRole : content.myRole,
							content.scope,
							content.message)
			}
		}
	}

	Component {
		id: endDialogComp

		MessageDialog {
			id: endDialog
			property string rowId: ""
			property string partnerName: ""
			width: Style.sizeHintM
			title: qsTr("End partnership")
			message: qsTr("End the partnership with \"%1\"? The relationship with this organization ends with it.").arg(endDialog.partnerName)
			onFinished: {
				var row = partnersPage.__rowById(endDialog.rowId)
				if (buttonId == Enums.yes && row && partnersPage.apiClient) {
					partnersPage.busyRowId = endDialog.rowId
					partnersPage.apiClient.removeConnection(row.connectionId, partnersPage.__tenantId)
				}
			}
		}
	}
}
