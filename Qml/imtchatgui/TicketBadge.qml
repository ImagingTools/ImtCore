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

	readonly property color badgeColor: {
		if (badgeType === "priority") {
			switch (value) {
				case 0: return "#4CAF50";  // Low - green
				case 1: return "#FF9800";  // Medium - orange
				case 2: return "#F44336";  // High - red
				case 3: return "#9C27B0";  // Critical - purple
				default: return Style.textSecondaryColor;
			}
		} else {
			// status
			switch (value) {
				case 0: return "#2196F3";  // Open - blue
				case 1: return "#FF9800";  // InProgress - orange
				case 2: return "#4CAF50";  // Resolved - green
				case 3: return "#9E9E9E";  // Closed - grey
				default: return Style.textSecondaryColor;
			}
		}
	}

	readonly property string badgeLabel: {
		if (badgeType === "priority") {
			switch (value) {
				case 0: return qsTr("Low");
				case 1: return qsTr("Med");
				case 2: return qsTr("High");
				case 3: return qsTr("Crit");
				default: return "?";
			}
		} else {
			switch (value) {
				case 0: return qsTr("Open");
				case 1: return qsTr("Active");
				case 2: return qsTr("Done");
				case 3: return qsTr("Closed");
				default: return "?";
			}
		}
	}

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
