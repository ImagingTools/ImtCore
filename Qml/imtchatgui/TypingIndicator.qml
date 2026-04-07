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
		anchors {
			left: parent.left
			verticalCenter: parent.verticalCenter
			leftMargin: Style.paddingM
		}
		spacing: Style.paddingXS

		Text {
			visible: typingIndicatorRoot.userName.length > 0
			text: typingIndicatorRoot.userName + " " + qsTr("is typing")
			font.pixelSize: Style.fontSizeXS
			color: Style.textSecondaryColor
			font.italic: true
			anchors.verticalCenter: parent.verticalCenter
		}

		// Animated dots
		Row {
			spacing: 3
			anchors.verticalCenter: parent.verticalCenter

			Repeater {
				model: 3

				Rectangle {
					id: dot
					width: 6
					height: 6
					radius: 3
					color: Style.textSecondaryColor

					property real phase: index * 0.4

					SequentialAnimation on opacity {
						running: typingIndicatorRoot.visible
						loops: Animation.Infinite

						PauseAnimation { duration: dot.phase * 1000 }
						NumberAnimation { from: 0.3; to: 1.0; duration: 400; easing.type: Easing.InOutSine }
						NumberAnimation { from: 1.0; to: 0.3; duration: 400; easing.type: Easing.InOutSine }
					}

					SequentialAnimation on y {
						running: typingIndicatorRoot.visible
						loops: Animation.Infinite

						PauseAnimation { duration: dot.phase * 1000 }
						NumberAnimation { from: 0; to: -4; duration: 300; easing.type: Easing.OutQuad }
						NumberAnimation { from: -4; to: 0; duration: 300; easing.type: Easing.InQuad }
					}
				}
			}
		}
	}
}
