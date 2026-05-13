// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: ticketViewRoot
	objectName: "TicketView"
	color: Style.backgroundColor

	property string ticketId: ""
	property int number: 0
	property string title: ""
	property string description: ""
	property int ticketType: 1
	property int status: 0
	property int stateReason: 0
	property int priority: 1
	property var assigneeIds: []
	property string assigneeName: ""
	property string reporterId: ""
	property string reporterName: ""
	property string conversationId: ""
	property string createdAt: ""
	property string updatedAt: ""
	property string closedAt: ""
	property var tags: []
	property var labelIds: []
	property var labels: []
	property bool locked: false
	property string lockReason: ""

	signal statusChangeRequested(int newStatus, int newStateReason)
	signal assigneeChangeRequested(var newAssigneeIds)
	signal conversationOpenRequested(string conversationId)
	signal ticketClosed()

	Flickable {
		anchors.fill: parent
		clip: true

		Column {
			width: ticketViewRoot.width
			spacing: Style.paddingM
			padding: Style.paddingL

			// Header: #number + title
			Column {
				width: parent.width - Style.paddingL * 2
				spacing: Style.paddingXS

				Row {
					width: parent.width
					spacing: Style.paddingS

					Text {
						text: "#" + ticketViewRoot.number
						font.pixelSize: Style.fontSizeL
						color: Style.textSecondaryColor
						visible: ticketViewRoot.number > 0
					}

					Text {
						text: ticketViewRoot.title
						font.pixelSize: Style.fontSizeL
						font.bold: true
						color: Style.textPrimaryColor
						wrapMode: Text.Wrap
						width: parent.width - (ticketViewRoot.number > 0 ? Style.paddingS + Style.fontSizeL * 3 : 0)
					}
				}

				// Status + State Reason badges row
				Row {
					spacing: Style.paddingXS

					TicketBadge {
						badgeType: "status"
						value: ticketViewRoot.status
					}

					TicketBadge {
						badgeType: "stateReason"
						value: ticketViewRoot.stateReason
						visible: ticketViewRoot.stateReason > 0
					}

					TicketBadge {
						badgeType: "priority"
						value: ticketViewRoot.priority
					}
				}

				// Lock indicator
				Row {
					visible: ticketViewRoot.locked
					spacing: Style.paddingXS

					Text {
						text: "🔒"
						font.pixelSize: Style.fontSizeXS
					}

					Text {
						text: ticketViewRoot.lockReason.length > 0
							? qsTr("Locked: %1").arg(ticketViewRoot.lockReason)
							: qsTr("This ticket is locked")
						font.pixelSize: Style.fontSizeXS
						color: Style.textSecondaryColor
					}
				}
			}

			// Labels row
			Row {
				visible: ticketViewRoot.labels.length > 0
				width: parent.width - Style.paddingL * 2
				spacing: Style.paddingXS

				Repeater {
					model: ticketViewRoot.labels

					Rectangle {
						height: Style.chipHeight
						width: labelText.width + Style.paddingM * 2
						radius: height / 2
						color: modelData.color || Style.tagBackgroundColor

						Text {
							id: labelText
							anchors.centerIn: parent
							text: modelData.name || ""
							font.pixelSize: Style.fontSizeXS
							color: "white"
							font.bold: true
						}
					}
				}
			}

			// Description
			Rectangle {
				width: parent.width - Style.paddingL * 2
				height: descText.height + Style.paddingM * 2
				color: Style.surfaceColor
				radius: Style.radiusS

				Text {
					id: descText
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.top: parent.top
					anchors.margins: Style.paddingM
					text: ticketViewRoot.description.length > 0
						? ticketViewRoot.description
						: qsTr("(No description provided)")
					font.pixelSize: Style.fontSizeS
					color: ticketViewRoot.description.length > 0
						? Style.textPrimaryColor
						: Style.textSecondaryColor
					wrapMode: Text.Wrap
				}
			}

			// Sidebar metadata
			Column {
				width: parent.width - Style.paddingL * 2
				spacing: Style.paddingS

				Row {
					spacing: Style.paddingL
					Text { text: qsTr("Type:"); font.pixelSize: Style.fontSizeXS; color: Style.textSecondaryColor; width: 100 }
					Text {
						text: ticketTypeLabel(ticketViewRoot.ticketType)
						font.pixelSize: Style.fontSizeXS
						color: Style.textPrimaryColor
					}
				}

				Row {
					spacing: Style.paddingL
					Text { text: qsTr("Reporter:"); font.pixelSize: Style.fontSizeXS; color: Style.textSecondaryColor; width: 100 }
					Text {
						text: ticketViewRoot.reporterName || ticketViewRoot.reporterId || "—"
						font.pixelSize: Style.fontSizeXS
						color: Style.textPrimaryColor
					}
				}

				Row {
					spacing: Style.paddingL
					Text { text: qsTr("Assignees:"); font.pixelSize: Style.fontSizeXS; color: Style.textSecondaryColor; width: 100 }
					Text {
						text: ticketViewRoot.assigneeName || (ticketViewRoot.assigneeIds.length > 0 ? ticketViewRoot.assigneeIds.join(", ") : "") || qsTr("No one assigned")
						font.pixelSize: Style.fontSizeXS
						color: Style.textPrimaryColor
					}
				}

				Row {
					spacing: Style.paddingL
					Text { text: qsTr("Created:"); font.pixelSize: Style.fontSizeXS; color: Style.textSecondaryColor; width: 100 }
					Text {
						text: ticketViewRoot.createdAt || "—"
						font.pixelSize: Style.fontSizeXS
						color: Style.textPrimaryColor
					}
				}

				Row {
					visible: ticketViewRoot.closedAt.length > 0
					spacing: Style.paddingL
					Text { text: qsTr("Closed:"); font.pixelSize: Style.fontSizeXS; color: Style.textSecondaryColor; width: 100 }
					Text {
						text: ticketViewRoot.closedAt
						font.pixelSize: Style.fontSizeXS
						color: Style.textPrimaryColor
					}
				}
			}

			// Tags
			Row {
				visible: ticketViewRoot.tags.length > 0
				width: parent.width - Style.paddingL * 2
				spacing: Style.paddingXS

				Repeater {
					model: ticketViewRoot.tags

					Rectangle {
						height: Style.chipHeight
						width: tagText.width + Style.paddingM * 2
						radius: height / 2
						color: Style.tagBackgroundColor

						Text {
							id: tagText
							anchors.centerIn: parent
							text: modelData
							font.pixelSize: Style.fontSizeXS
							color: Style.textPrimaryColor
						}
					}
				}
			}

			// Action buttons — GitHub Issues style: Close/Reopen
			Row {
				width: parent.width - Style.paddingL * 2
				spacing: Style.paddingS

				// Close ticket (with state reason)
				Rectangle {
					visible: ticketViewRoot.status === 0
					width: Style.buttonWidthM
					height: Style.buttonHeightM
					radius: Style.radiusS
					color: "#8957e5"

					Text {
						anchors.centerIn: parent
						text: qsTr("Close as completed")
						font.pixelSize: Style.fontSizeXS
						color: "white"
						font.bold: true
					}

					MouseArea {
						anchors.fill: parent
						onClicked: {
							ticketViewRoot.statusChangeRequested(1, 1)
						}
					}
				}

				// Close as not planned
				Rectangle {
					visible: ticketViewRoot.status === 0
					width: Style.buttonWidthM
					height: Style.buttonHeightM
					radius: Style.radiusS
					color: "transparent"
					border.color: Style.textSecondaryColor

					Text {
						anchors.centerIn: parent
						text: qsTr("Close as not planned")
						font.pixelSize: Style.fontSizeXS
						color: Style.textSecondaryColor
					}

					MouseArea {
						anchors.fill: parent
						onClicked: {
							ticketViewRoot.statusChangeRequested(1, 2)
						}
					}
				}

				// Reopen ticket
				Rectangle {
					visible: ticketViewRoot.status === 1
					width: Style.buttonWidthM
					height: Style.buttonHeightM
					radius: Style.radiusS
					color: "#1a7f37"

					Text {
						anchors.centerIn: parent
						text: qsTr("Reopen ticket")
						font.pixelSize: Style.fontSizeXS
						color: "white"
						font.bold: true
					}

					MouseArea {
						anchors.fill: parent
						onClicked: {
							ticketViewRoot.statusChangeRequested(0, 3)
						}
					}
				}

				// Open linked conversation
				Rectangle {
					visible: ticketViewRoot.conversationId.length > 0
					width: Style.buttonWidthM
					height: Style.buttonHeightM
					radius: Style.radiusS
					color: "transparent"
					border.color: Style.accentColor

					Text {
						anchors.centerIn: parent
						text: qsTr("Open Chat")
						font.pixelSize: Style.fontSizeXS
						color: Style.accentColor
					}

					MouseArea {
						anchors.fill: parent
						onClicked: {
							ticketViewRoot.conversationOpenRequested(ticketViewRoot.conversationId)
						}
					}
				}
			}
		}
	}

	function ticketTypeLabel(type) {
		switch (type) {
			case 0: return qsTr("Access Request");
			case 1: return qsTr("Support Request");
			case 2: return qsTr("Feature Request");
			case 3: return qsTr("Bug Report");
			default: return qsTr("Unknown");
		}
	}

}
