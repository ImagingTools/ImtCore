import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Item {
	id: titleContainer;

	width: image.width * image.visible + titleText.width + titleText.anchors.leftMargin;
	height: Math.max(image.height, titleText.height)

	property string text: "Title"
	property alias icon: image
	property alias font: titleText.font
	property int textMargin: Style.marginM

	Image{
		id: image;

		anchors.verticalCenter: parent.verticalCenter

		width: Style.iconSizeM
		height: width

		sourceSize.width: width
		sourceSize.height: height

		source:  ""

		property bool isEmpty: source == ""
		visible: !isEmpty

	}


	BaseText {
		id: titleText;

		anchors.left: image.isEmpty ? parent.left : image.right;
		anchors.leftMargin: image.isEmpty ? 0 : titleContainer.textMargin;
		anchors.verticalCenter: parent.verticalCenter;

		font.pixelSize: Style.fontSizeL;
		font.bold: true

		text: titleContainer.text;

	}

}

