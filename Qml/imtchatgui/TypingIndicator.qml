// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
	id: typingIndicatorRoot
	objectName: "TypingIndicator"
	height: visible ? Style.typingIndicatorHeight : 0
	visible: false

	property string userName: ""

	Row {
		anchors.left: parent.left
		anchors.verticalCenter: parent.verticalCenter
		anchors.leftMargin: Style.paddingM
		spacing: Style.paddingXS

		Text {
			visible: typingIndicatorRoot.userName.length > 0
			text: typingIndicatorRoot.userName + " " + qsTr("is typing") + dotsText.text
			font.pixelSize: Style.fontSizeXS
			color: Style.textSecondaryColor
			font.italic: true
			anchors.verticalCenter: parent.verticalCenter
		}

		Text {
			id: dotsText
			visible: typingIndicatorRoot.userName.length === 0
			text: qsTr("typing") + dotsText.dots
			font.pixelSize: Style.fontSizeXS
			color: Style.textSecondaryColor
			font.italic: true
			anchors.verticalCenter: parent.verticalCenter

			property string dots: "."

			Timer {
				running: typingIndicatorRoot.visible
				interval: 500
				repeat: true
				onTriggered: {
					if (dotsText.dots.length >= 3) {
						dotsText.dots = ".";
					} else {
						dotsText.dots += ".";
					}
				}
			}
		}
	}
}
