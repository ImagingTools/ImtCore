// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0

Item {
	id: typingIndicatorRoot
	objectName: "TypingIndicator"
	height: visible ? Style.buttonHeight : 0
	visible: false

	property string userName: ""
	property string dots: "."

	Timer {
		running: typingIndicatorRoot.visible
		interval: 500
		repeat: true
		onTriggered: {
			typingIndicatorRoot.dots = typingIndicatorRoot.dots.length >= 3 ? "." : typingIndicatorRoot.dots + ".";
		}
	}

	Row {
		anchors.left: parent.left
		anchors.verticalCenter: parent.verticalCenter
		anchors.leftMargin: Style.paddingM
		spacing: Style.paddingS

		Text {
			visible: typingIndicatorRoot.userName.length > 0
			text: typingIndicatorRoot.userName + " " + qsTr("is typing") + typingIndicatorRoot.dots
			font.pixelSize: Style.fontSizeXS
			color: Style.buttonInactiveTextColor
			font.italic: true
			anchors.verticalCenter: parent.verticalCenter
		}

		Text {
			visible: typingIndicatorRoot.userName.length === 0
			text: qsTr("typing") + typingIndicatorRoot.dots
			font.pixelSize: Style.fontSizeXS
			color: Style.buttonInactiveTextColor
			font.italic: true
			anchors.verticalCenter: parent.verticalCenter
		}
	}
}
