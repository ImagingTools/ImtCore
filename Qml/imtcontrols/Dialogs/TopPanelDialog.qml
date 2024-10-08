import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: topPanelDialogContainer;

	width: 300;
	height: 40;

	color: Style.baseColor;

	property string title;
	property int titleFontSize: Style.fontSize_subtitle;
	property string titleFontFamily: Style.fontFamily;
	property color titleTextColor: Style.textColor;

	property bool hasIcon: true;
	property string iconSource: "../../../../" + Style.getIconPath("Icons/AppIcon", Icon.State.On, Icon.Mode.Normal);
	property string closeButtonIconSource: "../../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);

	signal closeButtonClicked(int buttonId);

	Image {
		id: iconDialog;

		anchors.left: topPanelDialogContainer.left;
		anchors.leftMargin: 10;
		anchors.verticalCenter: topPanelDialogContainer.verticalCenter;

		width: 20;
		height: 20;

		sourceSize.height: height;
		sourceSize.width: width;

		source: topPanelDialogContainer.iconSource;

		visible: topPanelDialogContainer.hasIcon;
	}

	Text {
		id: titleDialog;

		anchors.left: topPanelDialogContainer.hasIcon ? iconDialog.right : parent.left;
		anchors.leftMargin: topPanelDialogContainer.hasIcon ? 10 : 15;
		anchors.verticalCenter: topPanelDialogContainer.verticalCenter;
		anchors.right: closeButton.left;
		anchors.rightMargin: 10;

		color: topPanelDialogContainer.titleTextColor;
		font.family: topPanelDialogContainer.titleFontFamily;
		font.pixelSize: topPanelDialogContainer.titleFontSize;

		text: topPanelDialogContainer.title;

		elide: Text.ElideRight;
	}

	Button {
		id: closeButton;

		anchors.right: topPanelDialogContainer.right;
		anchors.verticalCenter: topPanelDialogContainer.verticalCenter;
		anchors.rightMargin: 15;

		width: 20;
		height: 20;

		iconSource: topPanelDialogContainer.closeButtonIconSource;

		onClicked: {
			topPanelDialogContainer.closeButtonClicked(Enums.cancel);
		}

		decorator: Component {
			id: buttonDecorator;

			ButtonDecorator {
				color: closeButton.hovered ? Style.buttonHoverColor : "transparent";
				border.width: 0;
			}
		}
	}
}
