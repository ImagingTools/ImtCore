import QtQuick 2.12
import Acf 1.0

TableCellDelegateBase {
	id: objectLinkDelegate

	property string text
	signal linkActivated()

	Text {
		id: linkText
		anchors.verticalCenter: parent.verticalCenter
		anchors.left: parent.left
		anchors.leftMargin: Style.marginM
		font.pixelSize: Style.fontSizeM
		font.family: Style.fontFamily
		elide: Text.ElideRight
		color: "#0b5ed7"
		text: objectLinkDelegate.text
	}
	
	MouseArea {
		anchors.fill: linkText
		hoverEnabled: true
		cursorShape: Qt.PointingHandCursor
		onClicked: {
			objectLinkDelegate.linkActivated()
		}
	}
}
