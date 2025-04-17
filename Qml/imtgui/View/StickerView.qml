import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

Rectangle {
	width: stickerText.width + 2* Style.sizeSmallMargin;
	height: 20;
	radius: Style.buttonRadius;
	property alias text: stickerText.text
	property alias textColor: stickerText.color
	property alias font: stickerText.font

	Text {
		id: stickerText;
		anchors.centerIn: parent;
		color: Style.baseColor;
		font.family:Style.fontFamily;
		font.pixelSize: Style.fontSizeNormal;
	}
}


