import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

DecoratorBase {
	id: topLeftPanelDecorator;

	width: content.width;

	// The corner used to claim the width of the navigation rail, or two hundred
	// pixels, whichever was larger - a fixed appetite that took the same room out
	// of a narrow window as out of a wide one, leaving the search box beside it
	// with nothing. It now takes what the title needs and no more than a third of
	// the bar, and never gives up the logo.
	readonly property int barWidth: baseElement && baseElement.topPanel ? baseElement.topPanel.width : 0;
	readonly property int minWidth: applicationIcon.width + 2 * Style.marginL;
	property int maxWidth: Math.max(minWidth,
									barWidth > 0 ? Math.round(barWidth * 0.35) : Style.sizeHintXXS);

	Item {
		id: content;
		// Where the title actually ends, rather than its length plus a guess:
		// the old sum left the icon out and so under-measured whenever there
		// was one.
		width: Math.min(topLeftPanelDecorator.maxWidth,
						applicationName.x + tempText.width + Style.marginM);
		height: topLeftPanelDecorator.height;

		Image {
			id: applicationIcon
			anchors.verticalCenter: content.verticalCenter;
			anchors.left: parent.left
			// The same margin the navigation rail gives its icons, so the logo
			// stands at the head of that column rather than beside it.
			anchors.leftMargin: Style.marginL
			height: Math.min(Style.iconSizeL, parent.height - Style.marginM)
			width: visible ? height : 0
			visible: source !== ''
			source: context && context.appIcon && context.appIcon !== '' ? context.appIcon : ''
			sourceSize.width: width;
			sourceSize.height: height;
		}

		// A title beside a rail of sixteen-pixel labels: twenty points bold was
		// shouting over everything under it.
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

