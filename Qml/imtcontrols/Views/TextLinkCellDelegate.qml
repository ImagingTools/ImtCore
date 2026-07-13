import QtQuick 2.12
import Acf 1.0

TableCellDelegateBase {
	id: objectLinkDelegate

	property string text
	property string imageSource
	signal linkActivated()

	Image {
		id: image

		anchors.verticalCenter: parent.verticalCenter
		anchors.left: parent.left
		anchors.leftMargin: Style.marginM
		width: visible ? 20 : 0
		height: width
		visible: source !== ""
		sourceSize.width: width
		sourceSize.height: height
		source: objectLinkDelegate.imageSource
	}

	Text {
		id: linkText
		anchors.verticalCenter: parent.verticalCenter
		anchors.left: image.visible ? image.right : parent.left
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
