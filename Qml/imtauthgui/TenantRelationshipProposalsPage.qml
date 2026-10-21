// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantRelationshipProposalsPage
 *
 * Open relationship proposals of the organization: incoming ones are approved
 * or rejected here, sent ones can be withdrawn until the partner answers.
 */
ViewBase {
	id: proposalsPage

	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor

	readonly property var tenantData: proposalsPage.model
	property var stateManager: null
	property var apiClient: null
	readonly property bool __canManage: proposalsPage.stateManager ? proposalsPage.stateManager.canManageOrganizationConnections : false
	readonly property string __tenantId: proposalsPage.tenantData ? (proposalsPage.tenantData.m_id || "") : ""

	property string processingProposalId: ""

	function updateGui() {
		if (proposalsPage.apiClient && proposalsPage.__tenantId !== "") {
			proposalsPage.apiClient.fetchConnections(proposalsPage.__tenantId)
			proposalsPage.apiClient.fetchTenantRelationships(proposalsPage.__tenantId)
			proposalsPage.apiClient.fetchRelationshipProposals(proposalsPage.__tenantId)
		}
	}

	// Notifications update the page only while it is shown; a hidden one reloads when shown.
	property bool __updatePending: false

	function __updateWhenVisible() {
		if (proposalsPage.visible)
			proposalsPage.updateGui()
		else
			proposalsPage.__updatePending = true
	}

	onVisibleChanged: {
		if (proposalsPage.visible && proposalsPage.__updatePending) {
			proposalsPage.__updatePending = false
			proposalsPage.updateGui()
		}
	}

	Component.onCompleted: {
		proposalsPage.updateGui()
	}

	function __partnerName(partnerId) {
		var connections = proposalsPage.apiClient ? proposalsPage.apiClient.connectionsModel : null
		if (connections) {
			for (var i = 0; i < connections.count; ++i) {
				if (connections.get(i).partnerId === partnerId) {
					return connections.get(i).partnerName
				}
			}
		}
		return partnerId
	}

	function __typeText(proposalType) {
		if (proposalType === "Update") {
			return qsTr("Change of the relationship")
		}
		if (proposalType === "Delete") {
			return qsTr("End of the relationship")
		}
		return qsTr("New relationship")
	}

	// Proposed roles belong to the relationship's source and target; a new relationship has the initiator as its source.
	function __isSource(proposal) {
		if (proposal.proposalType === "Create") {
			return proposal.initiatorTenantId === proposalsPage.__tenantId
		}
		var relationships = proposalsPage.apiClient ? proposalsPage.apiClient.tenantRelationshipsModel : null
		if (relationships) {
			for (var i = 0; i < relationships.count; ++i) {
				if (relationships.get(i).id === proposal.existingRelationshipId) {
					return relationships.get(i).sourceTenantId === proposalsPage.__tenantId
				}
			}
		}
		return proposal.initiatorTenantId === proposalsPage.__tenantId
	}

	// The initiator agrees by proposing, so an open proposal waits for the counterparty.
	function refreshLists() {
		var proposals = proposalsPage.apiClient ? proposalsPage.apiClient.relationshipProposalsModel : null
		var incoming = []
		var outgoing = []
		if (proposals) {
			for (var i = 0; i < proposals.count; ++i) {
				var proposal = proposals.get(i)
				if (proposal.status !== "ApprovedByInitiator" && proposal.status !== "Pending") {
					continue
				}
				var isInitiator = proposal.initiatorTenantId === proposalsPage.__tenantId
				var entry = {
					"id": proposal.id,
					"partnerName": proposalsPage.__partnerName(isInitiator ? proposal.counterpartyTenantId : proposal.initiatorTenantId),
					"typeText": proposalsPage.__typeText(proposal.proposalType),
					"roles": proposalsPage.__isSource(proposal)
							 ? proposal.proposedSourceRole + " \u2194 " + proposal.proposedTargetRole
							 : proposal.proposedTargetRole + " \u2194 " + proposal.proposedSourceRole,
					"description": proposal.proposedDescription
				}
				if (isInitiator) {
					outgoing.push(entry)
				}
				else {
					incoming.push(entry)
				}
			}
		}
		incomingList.model = incoming
		outgoingList.model = outgoing
	}

	Connections {
		target: proposalsPage.apiClient

		function onRelationshipProposalsReceived() {
			proposalsPage.refreshLists()
		}

		function onConnectionsReceived() {
			proposalsPage.refreshLists()
		}

		function onTenantRelationshipsReceived() {
			proposalsPage.refreshLists()
		}

		function onRelationshipProposalApproved(relationshipId) {
			proposalsPage.processingProposalId = ""
			PopupManager.addSuccessMessage(qsTr("Relationship proposal approved"), true)
			proposalsPage.updateGui()
		}

		function onRelationshipProposalRejected() {
			proposalsPage.processingProposalId = ""
			PopupManager.addSuccessMessage(qsTr("Relationship proposal rejected"), true)
			proposalsPage.updateGui()
		}

		function onRelationshipProposalCanceled() {
			proposalsPage.processingProposalId = ""
			PopupManager.addSuccessMessage(qsTr("Relationship proposal withdrawn"), true)
			proposalsPage.updateGui()
		}

		function onRelationshipProposalCreated() {
			proposalsPage.updateGui()
		}

		function onRequestFailed(message) {
			proposalsPage.processingProposalId = ""
		}

		function onSubscriptionConnectionCodesChanged(notification) {
			proposalsPage.__updateWhenVisible()
		}

		function onSubscriptionsResumed() {
			proposalsPage.__updateWhenVisible()
		}
	}

	Flickable {
		anchors.fill: parent
		anchors.topMargin: Style.marginXL
		contentHeight: mainColumn.height + Style.marginXL
		clip: true

		Column {
			id: mainColumn
			anchors.horizontalCenter: parent.horizontalCenter
			width: Math.min(parent.width - 2 * Style.marginXL, Style.contentWidthMax)
			spacing: Style.marginM

			GroupHeaderView {
				width: parent.width
				title: qsTr("Incoming Proposals")
			}

			Text {
				width: parent.width
				wrapMode: Text.WordWrap
				text: qsTr("A relationship comes into force or changes only when both organizations agree.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			Rectangle {
				width: parent.width
				height: incomingList.count === 0 ? Style.controlHeightL + 2 * Style.marginM : incomingList.height + 2
				color: Style.baseColor
				radius: Style.radiusM
				border.width: 1
				border.color: Style.borderColor
				clip: true

				Text {
					anchors.left: parent.left
					anchors.leftMargin: Style.marginL
					anchors.verticalCenter: parent.verticalCenter
					visible: incomingList.count === 0
					text: qsTr("No proposals are waiting for your answer.")
					font.pixelSize: Style.fontSizeM
					color: Style.inactiveTextColor
				}

				ListView {
					id: incomingList
					anchors.top: parent.top
					anchors.topMargin: 1
					x: 1
					width: parent.width - 2
					height: incomingList.contentHeight
					interactive: false

					delegate: Rectangle {
						width: incomingList.width
						height: incomingContent.height + Style.marginM * 2
						color: incomingMouseArea.containsMouse ? Style.hover : "transparent"

						MouseArea {
							id: incomingMouseArea
							anchors.fill: parent
							hoverEnabled: true
						}

						Rectangle {
							anchors.top: parent.top
							anchors.left: parent.left
							anchors.leftMargin: Style.marginL
							anchors.right: parent.right
							anchors.rightMargin: Style.marginL
							height: 1
							color: Style.borderColor
							visible: index > 0
						}

						Column {
							id: incomingContent
							anchors.left: parent.left
							anchors.right: incomingActions.left
							anchors.leftMargin: Style.marginL
							anchors.rightMargin: Style.marginS
							anchors.verticalCenter: parent.verticalCenter
							spacing: Style.marginXS

							Text {
								text: modelData.partnerName
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: Style.textColor
							}

							Text {
								text: modelData.typeText + ": " + modelData.roles
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}

							Text {
								visible: modelData.description !== ""
								text: modelData.description
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
						}

						Row {
							id: incomingActions
							anchors.right: parent.right
							anchors.rightMargin: Style.marginL
							anchors.verticalCenter: parent.verticalCenter
							spacing: Style.marginS
							visible: proposalsPage.__canManage

							Button {
								text: qsTr("Approve")
								variant: "primary"
								enabled: proposalsPage.processingProposalId === ""
								onClicked: {
									proposalsPage.processingProposalId = modelData.id
									proposalsPage.apiClient.approveRelationshipProposal(modelData.id, proposalsPage.__tenantId)
								}
							}

							Button {
								text: qsTr("Reject")
								enabled: proposalsPage.processingProposalId === ""
								onClicked: {
									proposalsPage.processingProposalId = modelData.id
									proposalsPage.apiClient.rejectRelationshipProposal(modelData.id, proposalsPage.__tenantId)
								}
							}
						}
					}
				}
			}

			Item {
				width: parent.width
				height: Style.marginL
			}

			GroupHeaderView {
				width: parent.width
				title: qsTr("Sent Proposals")
			}

			Rectangle {
				width: parent.width
				height: outgoingList.count === 0 ? Style.controlHeightL + 2 * Style.marginM : outgoingList.height + 2
				color: Style.baseColor
				radius: Style.radiusM
				border.width: 1
				border.color: Style.borderColor
				clip: true

				Text {
					anchors.left: parent.left
					anchors.leftMargin: Style.marginL
					anchors.verticalCenter: parent.verticalCenter
					visible: outgoingList.count === 0
					text: qsTr("No proposals are waiting for the partner's answer.")
					font.pixelSize: Style.fontSizeM
					color: Style.inactiveTextColor
				}

				ListView {
					id: outgoingList
					anchors.top: parent.top
					anchors.topMargin: 1
					x: 1
					width: parent.width - 2
					height: outgoingList.contentHeight
					interactive: false

					delegate: Rectangle {
						width: outgoingList.width
						height: outgoingContent.height + Style.marginM * 2
						color: outgoingMouseArea.containsMouse ? Style.hover : "transparent"

						MouseArea {
							id: outgoingMouseArea
							anchors.fill: parent
							hoverEnabled: true
						}

						Rectangle {
							anchors.top: parent.top
							anchors.left: parent.left
							anchors.leftMargin: Style.marginL
							anchors.right: parent.right
							anchors.rightMargin: Style.marginL
							height: 1
							color: Style.borderColor
							visible: index > 0
						}

						Column {
							id: outgoingContent
							anchors.left: parent.left
							anchors.right: outgoingActions.left
							anchors.leftMargin: Style.marginL
							anchors.rightMargin: Style.marginS
							anchors.verticalCenter: parent.verticalCenter
							spacing: Style.marginXS

							Text {
								text: modelData.partnerName
								font.pixelSize: Style.fontSizeM
								font.bold: true
								color: Style.textColor
							}

							Text {
								text: modelData.typeText + ": " + modelData.roles
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}

							Text {
								visible: modelData.description !== ""
								text: modelData.description
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
						}

						Row {
							id: outgoingActions
							anchors.right: parent.right
							anchors.rightMargin: Style.marginL
							anchors.verticalCenter: parent.verticalCenter
							spacing: Style.marginS
							visible: proposalsPage.__canManage

							Button {
								text: qsTr("Withdraw")
								enabled: proposalsPage.processingProposalId === ""
								onClicked: {
									proposalsPage.processingProposalId = modelData.id
									proposalsPage.apiClient.cancelRelationshipProposal(modelData.id, proposalsPage.__tenantId)
								}
							}
						}
					}
				}
			}
		}
	}
}
