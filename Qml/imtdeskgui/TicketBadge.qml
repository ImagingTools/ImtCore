// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: ticketBadgeRoot
	objectName: "TicketBadge"

	// badgeType: "status" | "priority"
	property string badgeType: "status"
	// value: integer matching TicketStatus or TicketPriority enum
	property int value: 0

	readonly property var badgeColors: badgeType === "priority"
		? ["#4CAF50", "#FF9800", "#F44336", "#9C27B0"]
		: ["#2196F3", "#FF9800", "#4CAF50", "#9E9E9E"]

	readonly property var badgeLabels: badgeType === "priority"
		? [qsTr("Low"), qsTr("Med"), qsTr("High"), qsTr("Crit")]
		: [qsTr("Open"), qsTr("Active"), qsTr("Done"), qsTr("Closed")]

	readonly property color badgeColor: (value >= 0 && value < badgeColors.length)
		? badgeColors[value] : Style.textSecondaryColor

	readonly property string badgeLabel: (value >= 0 && value < badgeLabels.length)
		? badgeLabels[value] : "?"

	width: badgeLabelText.implicitWidth + Style.paddingXS * 2 + 4
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
