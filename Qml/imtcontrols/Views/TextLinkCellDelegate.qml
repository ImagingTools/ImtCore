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
		visible: objectLinkDelegate.imageSource !== ""
		sourceSize.width: width
		sourceSize.height: height
		source: objectLinkDelegate.imageSource
	}

	Text {
		id: linkText
		anchors.verticalCenter: parent.verticalCenter
		anchors.left: image.visible ? image.right : parent.left
		anchors.leftMargin: objectLinkDelegate.rowDelegate ? objectLinkDelegate.rowDelegate.textLeftMargin : Style.marginS
		anchors.right: parent.right
		anchors.rightMargin: objectLinkDelegate.rowDelegate ? objectLinkDelegate.rowDelegate.textRightMargin : Style.marginS
		font.pixelSize: Style.fontSizeM
		font.family: Style.fontFamily
		elide: Text.ElideRight
		color: Style.linkColor
		text: objectLinkDelegate.text
	}
	
	MouseArea {
		anchors.left: linkText.left
		anchors.verticalCenter: linkText.verticalCenter
		width: Math.min(linkText.contentWidth, linkText.width)
		height: linkText.height
		hoverEnabled: true
		cursorShape: Qt.PointingHandCursor
		onClicked: {
			objectLinkDelegate.linkActivated()
		}
	}
}
