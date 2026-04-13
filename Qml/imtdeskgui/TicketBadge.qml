// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: ticketBadgeRoot
	objectName: "TicketBadge"

	// badgeType: "status" | "priority" | "stateReason" | "label"
	property string badgeType: "status"
	// value: integer matching TicketStatus, TicketPriority, or StateReason enum
	property int value: 0
	// labelColor: hex color for label badges
	property string labelColor: ""
	// labelName: text for label badges
	property string labelName: ""

	readonly property string badgeColor: {
		if (badgeType === "label" && labelColor.length > 0) {
			return labelColor
		}
		if (badgeType === "priority") {
			let colors = ["#4CAF50", "#FF9800", "#F44336", "#9C27B0"]
			return (value >= 0 && value < colors.length) ? colors[value] : Style.textSecondaryColor
		}
		if (badgeType === "stateReason") {
			let colors = ["#9E9E9E", "#8957e5", "#9E9E9E", "#2196F3"]
			return (value >= 0 && value < colors.length) ? colors[value] : Style.textSecondaryColor
		}
		// status: Open = green (#1a7f37), Closed = purple (#8957e5)
		let colors = ["#1a7f37", "#8957e5"]
		return (value >= 0 && value < colors.length) ? colors[value] : Style.textSecondaryColor
	}

	readonly property string badgeLabel: {
		if (badgeType === "label") {
			return labelName
		}
		if (badgeType === "priority") {
			let labels = [qsTr("Low"), qsTr("Medium"), qsTr("High"), qsTr("Critical")]
			return (value >= 0 && value < labels.length) ? labels[value] : "?"
		}
		if (badgeType === "stateReason") {
			let labels = ["", qsTr("Completed"), qsTr("Not planned"), qsTr("Reopened")]
			return (value >= 0 && value < labels.length) ? labels[value] : ""
		}
		// status: Open / Closed
		let labels = [qsTr("Open"), qsTr("Closed")]
		return (value >= 0 && value < labels.length) ? labels[value] : "?"
	}

	width: Style.badgeWidthS
	height: Style.badgeHeight
	radius: height / 2
	color: badgeColor

	Text {
		id: badgeLabelText
		anchors.centerIn: parent
		text: ticketBadgeRoot.badgeLabel
		font.pixelSize: Style.fontSizeXS
		color: "white"
		font.bold: true
	}
}
