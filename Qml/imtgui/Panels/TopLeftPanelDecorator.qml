import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

DecoratorBase {
	id: topLeftPanelDecorator;

	width: content.width;

	readonly property int barWidth: baseElement && baseElement.topPanel ? baseElement.topPanel.width : 0;
	readonly property int minWidth: applicationIcon.width + 2 * Style.marginL;
	property int maxWidth: Math.max(minWidth,
									barWidth > 0 ? Math.round(barWidth * 0.35) : Style.sizeHintXXS);

	Item {
		id: content;
		width: Math.min(topLeftPanelDecorator.maxWidth,
						applicationName.x + tempText.width + Style.marginM);
		height: topLeftPanelDecorator.height;

		Image {
			id: applicationIcon
			anchors.verticalCenter: content.verticalCenter;
			anchors.left: parent.left
			anchors.leftMargin: Style.marginL
			height: Math.min(Style.fontSizeXXXL, parent.height - Style.marginM)
			width: visible ? height : 0
			visible: source !== ''
			source: context && context.appIcon && context.appIcon !== '' ? context.appIcon
				: "qrc:/" + Style.getLogoIconPath(Icon.State.On, Icon.Mode.Normal)
			sourceSize.width: width;
			sourceSize.height: height;
		}

		Text {
			id: applicationName;
			anchors.verticalCenter: content.verticalCenter;
			anchors.left: applicationIcon.visible ? applicationIcon.right : parent.left;
			anchors.leftMargin: applicationIcon.visible ? Style.marginM : Style.marginL;
			anchors.right: parent.right;
			anchors.rightMargin: Style.marginM
			font.family: Style.fontFamilyBold;
			font.pixelSize: Style.fontSizeXL;
			color: Style.titleColor;
			text: context && context.appName && context.appName !== "" ? context.appName : "";
			elide: Text.ElideRight;
		}
	}

	Text {
		id: tempText;
		font.family: Style.fontFamilyBold;
		font.pixelSize: applicationName.font.pixelSize;
		text: applicationName.text;
		visible: false;
	}
}

