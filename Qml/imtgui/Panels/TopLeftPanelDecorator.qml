import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

DecoratorBase {
    id: topLeftPanelDecorator;

	width: content.width;

	property int maxWidth: menuWidth < Style.sizeHintXXS ? Style.sizeHintXXS : menuWidth;
	property int menuWidth: width
	property bool hasCollapseButton: Style.enableMenuPanelCollapse

	Component.onCompleted: {
		Events.subscribeEvent("MenuWidthChanged", onMenuWidthChanged);
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("MenuWidthChanged", onMenuWidthChanged);
	}

	function onMenuWidthChanged(widthArg){
		menuWidth = widthArg
	}

    Item {
        id: content;
		width: topLeftPanelDecorator.hasCollapseButton ? topLeftPanelDecorator.menuWidth : tempText.x + tempText.width + 2*Style.marginM > topLeftPanelDecorator.maxWidth ?
                   topLeftPanelDecorator.maxWidth:
				   tempText.x + tempText.width + 2*Style.marginM;
        height: topLeftPanelDecorator.height;

        Image {
            id: applicationIcon
            anchors.verticalCenter: content.verticalCenter;
            anchors.left: parent.left
            anchors.leftMargin: Style.marginM
            height: parent.height - Style.marginM
            width: visible ? height : 0
            visible: source !== ''
            source: context && context.appIcon && context.appIcon !== '' ? context.appIcon : ''
            sourceSize.width: width;
            sourceSize.height: height;
        }

        Text {
            id: applicationName;
            anchors.verticalCenter: content.verticalCenter;
            anchors.left: applicationIcon.visible ? applicationIcon.right : parent.left;
            anchors.leftMargin: Style.marginM;
            anchors.right: parent.right;
			anchors.rightMargin: Style.marginM + menuButton.visible * (topLeftPanelDecorator.menuWidth - menuButton.x);
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSizeXXL;
            color: Style.textColor;
			text: context && context.appName && context.appName !== "" ? context.appName : "";
            elide: Text.ElideRight;
        }
    }

	ToolButton{
		id: menuButton

		anchors.verticalCenter: content.verticalCenter;
		x: collapsed ? (topLeftPanelDecorator.menuWidth - width)/2 : topLeftPanelDecorator.menuWidth - width - Style.marginM

		visible: Style.enableMenuPanelCollapse

		width: Style.controlHeightS
		height: width

		iconSource: "../../../../" + Style.getIconPath(collapsed ? "Icons/Right" : "Icons/Left", Icon.State.Off, Icon.Mode.Normal);

		property bool collapsed: false

		onClicked: {
			if(!collapsed){
				Events.sendEvent("CollapseMenu", true)
			}
			else {
				Events.sendEvent("ExpandMenu", false)
			}
			collapsed = !collapsed
			content.visible = !collapsed
		}
	}

    Text {
        id: tempText;
        font.family: Style.fontFamilyBold;
        font.pixelSize: Style.fontSizeXXL;
        color: Style.textColor;
        text: applicationName.text;
        visible: false;
    }
}

