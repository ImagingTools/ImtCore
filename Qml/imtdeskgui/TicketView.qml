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
	property string title: ""
	property string description: ""
	property int ticketType: 1
	property int status: 0
	property int priority: 1
	property string assigneeId: ""
	property string assigneeName: ""
	property string reporterId: ""
	property string reporterName: ""
	property string conversationId: ""
	property string createdAt: ""
	property string resolvedAt: ""
	property int environment: 2
	property var tags: []

	signal statusChangeRequested(int newStatus)
	signal assigneeChangeRequested(string newAssigneeId)
	signal conversationOpenRequested(string conversationId)
	signal ticketClosed()

	ScrollView {
		anchors.fill: parent
		clip: true

		Column {
			width: ticketViewRoot.width
			spacing: Style.paddingM
			padding: Style.paddingL

			// Header
			Row {
				width: parent.width - Style.paddingL * 2
				spacing: Style.paddingM

				Column {
					width: parent.width - statusBadgeRow.width - Style.paddingM
					spacing: Style.paddingXS

					Text {
						text: ticketViewRoot.title
						font.pixelSize: Style.fontSizeL
						font.bold: true
						color: Style.textPrimaryColor
						wrapMode: Text.Wrap
						width: parent.width
					}

					Text {
						text: qsTr("Created: %1").arg(ticketViewRoot.createdAt)
						font.pixelSize: Style.fontSizeXS
						color: Style.textSecondaryColor
					}
				}

				Row {
					id: statusBadgeRow
					spacing: Style.paddingXS

					TicketBadge {
						badgeType: "status"
						value: ticketViewRoot.status
					}

					TicketBadge {
						badgeType: "priority"
						value: ticketViewRoot.priority
					}
				}
			}

			// Description
			Rectangle {
				width: parent.width - Style.paddingL * 2
				height: descText.implicitHeight + Style.paddingM * 2
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

			// Metadata grid
			Grid {
				width: parent.width - Style.paddingL * 2
				columns: 2
				columnSpacing: Style.paddingL
				rowSpacing: Style.paddingS

				Text { text: qsTr("Type:"); font.pixelSize: Style.fontSizeXS; color: Style.textSecondaryColor }
				Text {
					text: ticketTypeLabel(ticketViewRoot.ticketType)
					font.pixelSize: Style.fontSizeXS
					color: Style.textPrimaryColor
				}

				Text { text: qsTr("Environment:"); font.pixelSize: Style.fontSizeXS; color: Style.textSecondaryColor }
				Text {
					text: environmentLabel(ticketViewRoot.environment)
					font.pixelSize: Style.fontSizeXS
					color: Style.textPrimaryColor
				}

				Text { text: qsTr("Reporter:"); font.pixelSize: Style.fontSizeXS; color: Style.textSecondaryColor }
				Text {
					text: ticketViewRoot.reporterName || ticketViewRoot.reporterId || "—"
					font.pixelSize: Style.fontSizeXS
					color: Style.textPrimaryColor
				}

				Text { text: qsTr("Assignee:"); font.pixelSize: Style.fontSizeXS; color: Style.textSecondaryColor }
				Text {
					text: ticketViewRoot.assigneeName || ticketViewRoot.assigneeId || qsTr("Unassigned")
					font.pixelSize: Style.fontSizeXS
					color: Style.textPrimaryColor
				}
			}

			// Tags
			Flow {
				visible: ticketViewRoot.tags.length > 0
				width: parent.width - Style.paddingL * 2
				spacing: Style.paddingXS

				Repeater {
					model: ticketViewRoot.tags

					Rectangle {
						height: Style.chipHeight
						width: tagText.implicitWidth + Style.paddingS * 2
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

			// Action buttons
			Row {
				width: parent.width - Style.paddingL * 2
				spacing: Style.paddingS

				// Status change button
				Rectangle {
					visible: ticketViewRoot.status < 3
					width: Style.buttonWidthM
					height: Style.buttonHeightM
					radius: Style.radiusS
					color: Style.accentColor

					Text {
						anchors.centerIn: parent
						text: nextStatusLabel(ticketViewRoot.status)
						font.pixelSize: Style.fontSizeXS
						color: "white"
						font.bold: true
					}

					MouseArea {
						anchors.fill: parent
						onClicked: ticketViewRoot.statusChangeRequested(ticketViewRoot.status + 1)
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
						onClicked: ticketViewRoot.conversationOpenRequested(ticketViewRoot.conversationId)
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

	function environmentLabel(env) {
		switch (env) {
			case 0: return qsTr("Development");
			case 1: return qsTr("Staging");
			case 2: return qsTr("Production");
			default: return qsTr("Unknown");
		}
	}

	function nextStatusLabel(currentStatus) {
		switch (currentStatus) {
			case 0: return qsTr("Start Progress");
			case 1: return qsTr("Mark Resolved");
			case 2: return qsTr("Close Ticket");
			default: return qsTr("Update");
		}
	}
}
