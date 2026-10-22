import QtQuick 2.12

import Acf 1.0
import imtcontrols 1.0

Rectangle {
	id: root

	anchors.fill: parent
	clip: true
	color: Style.baseColor

	Text {
		anchors.centerIn: parent
		text: qsTr("Reports")
		color: Style.textColor
		font.pixelSize: Style.fontSizeXLarge
		font.family: Style.fontFamily
	}
}
