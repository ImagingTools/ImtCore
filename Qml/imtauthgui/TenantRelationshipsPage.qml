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
 * TenantRelationshipsPage
 *
 * Displays active relationships and pending relationship proposals.
 * Relationships are created via the Relationship Proposal flow (two-sided approval).
 * Remove is available for active relationships.
 */
ViewBase {
	id: relationshipsPage

	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor

	readonly property var tenantData: relationshipsPage.model
	property var stateManager: null
	property var apiClient: null

	function updateGui() {
		if (relationshipsPage.apiClient && relationshipsPage.tenantData
				&& relationshipsPage.tenantData.m_id) {
			relationshipsPage.apiClient.fetchTenantRelationships(relationshipsPage.tenantData.m_id)
			relationshipsPage.apiClient.fetchRelationshipProposals(relationshipsPage.tenantData.m_id)
		}
	}

	Component.onCompleted: {
		updateGui()
	}

	Connections {
		target: relationshipsPage.apiClient

		function onTenantRelationshipsReceived() {
			relationshipsList.model = relationshipsPage.apiClient.tenantRelationshipsModel
		}

		function onRelationshipProposalsReceived() {
			proposalsList.model = relationshipsPage.apiClient.relationshipProposalsModel
		}

		function onTenantRelationshipRemoved() {
			PopupManager.addSuccessMessage(qsTr("Relationship removed"), true)
			updateGui()
		}

		function onRelationshipProposalCreated() {
			PopupManager.addSuccessMessage(qsTr("Relationship proposal sent"), true)
			updateGui()
		}

		function onRelationshipProposalApproved(relationshipId) {
			PopupManager.addSuccessMessage(
				relationshipId ? qsTr("Relationship established") : qsTr("Proposal approved (awaiting counterparty)"), true)
			updateGui()
		}

		function onRelationshipProposalRejected() {
			PopupManager.addSuccessMessage(qsTr("Proposal rejected"), true)
			updateGui()
		}
	}

	Column {
		anchors.fill: parent
		anchors.margins: Style.marginL
		spacing: Style.marginL

		Text {
			text: qsTr("Relationships")
			font.pixelSize: Style.fontSizeH2
			font.bold: true
			color: Style.textColor
		}

		Text {
			text: qsTr("Business relationships with connected organizations. Creating or modifying a relationship requires approval from both parties.")
			font.pixelSize: Style.fontSizeDefault
			color: Style.textSecondaryColor
			wrapMode: Text.WordWrap
			width: parent.width
		}

		// --- Pending Proposals Section ---
		Text {
			visible: proposalsList.count > 0
			text: qsTr("Pending Proposals")
			font.pixelSize: Style.fontSizeH3
			font.bold: true
			color: Style.textColor
		}

		ListView {
			id: proposalsList
			width: parent.width
			height: Math.min(contentHeight, 200)
			clip: true
			spacing: Style.marginS
			visible: count > 0

			delegate: Rectangle {
				width: proposalsList.width
				height: proposalCol.height + Style.marginM * 2
				color: Style.highlightPanelColor
				radius: Style.radiusS

				Column {
					id: proposalCol
					anchors.left: parent.left
					anchors.right: proposalActions.left
					anchors.margins: Style.marginM
					anchors.verticalCenter: parent.verticalCenter
					spacing: Style.marginXS

					Text {
						text: {
							var typeStr = modelData.proposalType === "Create" ? qsTr("New Relationship")
								: modelData.proposalType === "Update" ? qsTr("Update Relationship")
								: qsTr("Remove Relationship")
							return typeStr + " — " + (modelData.counterpartyName || modelData.counterpartyTenantId || "")
						}
						font.pixelSize: Style.fontSizeDefault
						font.bold: true
						color: Style.textColor
					}

					Text {
						text: qsTr("Status: %1").arg(modelData.status || "")
						font.pixelSize: Style.fontSizeSmall
						color: Style.textSecondaryColor
					}

					Text {
						visible: modelData.message && modelData.message !== ""
						text: modelData.message || ""
						font.pixelSize: Style.fontSizeSmall
						color: Style.textSecondaryColor
					}
				}

				Row {
					id: proposalActions
					anchors.right: parent.right
					anchors.rightMargin: Style.marginM
					anchors.verticalCenter: parent.verticalCenter
					spacing: Style.marginS

					Button {
						text: qsTr("Approve")
						visible: modelData.counterpartyTenantId === (relationshipsPage.tenantData ? relationshipsPage.tenantData.m_id : "")
							&& modelData.status === "ApprovedByInitiator"
						onClicked: {
							if (relationshipsPage.apiClient && relationshipsPage.tenantData) {
								relationshipsPage.apiClient.approveRelationshipProposal(
									modelData.id, relationshipsPage.tenantData.m_id)
							}
						}
					}

					Button {
						text: qsTr("Reject")
						visible: modelData.status !== "Applied" && modelData.status !== "Rejected" && modelData.status !== "Canceled"
						onClicked: {
							if (relationshipsPage.apiClient && relationshipsPage.tenantData) {
								relationshipsPage.apiClient.rejectRelationshipProposal(
									modelData.id, relationshipsPage.tenantData.m_id)
							}
						}
					}
				}
			}
		}

		// --- Active Relationships Section ---
		Text {
			text: qsTr("Active Relationships")
			font.pixelSize: Style.fontSizeH3
			font.bold: true
			color: Style.textColor
		}

		Button {
			text: qsTr("+ Create Relationship Proposal")
			visible: relationshipsPage.apiClient
				&& relationshipsPage.apiClient.connectionsModel
				&& relationshipsPage.apiClient.connectionsModel.count > 0
			onClicked: {
				proposalCreator.visible = true
			}
		}

		ListView {
			id: relationshipsList
			width: parent.width
			height: parent.height - y - Style.marginL
			clip: true
			spacing: Style.marginS

			delegate: Rectangle {
				width: relationshipsList.width
				height: relCol.height + Style.marginM * 2
				color: Style.panelColor
				radius: Style.radiusS

				Column {
					id: relCol
					anchors.left: parent.left
					anchors.right: relRemoveBtn.left
					anchors.margins: Style.marginM
					anchors.verticalCenter: parent.verticalCenter
					spacing: Style.marginXS

					Text {
						text: (modelData.targetTenantName || modelData.targetTenantId || "") +
							" — " + (modelData.sourceRole || "") + " / " + (modelData.targetRole || "")
						font.pixelSize: Style.fontSizeDefault
						font.bold: true
						color: Style.textColor
					}

					Text {
						visible: modelData.description && modelData.description !== ""
						text: modelData.description || ""
						font.pixelSize: Style.fontSizeSmall
						color: Style.textSecondaryColor
					}

					Text {
						text: qsTr("Since: %1").arg(
							modelData.createdAt ? new Date(modelData.createdAt).toLocaleDateString() : "—")
						font.pixelSize: Style.fontSizeSmall
						color: Style.textSecondaryColor
					}
				}

				Button {
					id: relRemoveBtn
					text: qsTr("Remove")
					anchors.right: parent.right
					anchors.rightMargin: Style.marginM
					anchors.verticalCenter: parent.verticalCenter
					onClicked: {
						if (relationshipsPage.apiClient && relationshipsPage.tenantData) {
							relationshipsPage.apiClient.removeTenantRelationship(
								relationshipsPage.tenantData.m_id, modelData.id)
						}
					}
				}
			}

			Text {
				visible: relationshipsList.count === 0
				text: qsTr("No active relationships. Create a relationship proposal to establish one with a connected organization.")
				font.pixelSize: Style.fontSizeDefault
				color: Style.textSecondaryColor
				anchors.centerIn: parent
				wrapMode: Text.WordWrap
			}
		}
	}
}

