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

	readonly property var _colorMap: ({
		"status":      [Style.statusOpenColor, Style.statusClosedColor],
		"priority":    [Style.priorityLowColor, Style.priorityMediumColor, Style.priorityHighColor, Style.priorityCriticalColor],
		"stateReason": [Style.indicatorInactiveColor, Style.statusClosedColor, Style.indicatorInactiveColor, Style.linkColor]
	})

	readonly property var _labelMap: ({
		"status":      [qsTr("Open"), qsTr("Closed")],
		"priority":    [qsTr("Low"), qsTr("Medium"), qsTr("High"), qsTr("Critical")],
		"stateReason": ["", qsTr("Completed"), qsTr("Not planned"), qsTr("Reopened")]
	})

	readonly property string badgeColor: badgeType === "label" ? labelColor
		: (_colorMap[badgeType] && value >= 0 && value < _colorMap[badgeType].length)
			? _colorMap[badgeType][value] : Style.buttonInactiveTextColor

	readonly property string badgeLabel: badgeType === "label" ? labelName
		: (_labelMap[badgeType] && value >= 0 && value < _labelMap[badgeType].length)
			? _labelMap[badgeType][value] : "?"

	width: badgeLabelText.width + 2*Style.marginM
	height: 30
	radius: height / 2
	color: badgeColor

	Text {
		id: badgeLabelText
		anchors.centerIn: parent
		text: ticketBadgeRoot.badgeLabel
		font.pixelSize: Style.fontSizeM
		color: "white"
		font.bold: true
	}
}
