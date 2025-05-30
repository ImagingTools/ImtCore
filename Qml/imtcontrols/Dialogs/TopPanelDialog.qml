import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: topPanelDialogContainer;

	width: Style.sizeHintXS;
	height: Style.controlHeightL;
	radius: Style.radiusXS;

	color: Style.baseColor;

	property string title;
	property int titleFontSize: Style.fontSizeXL;
	property string titleFontFamily: Style.fontFamily;
	property color titleTextColor: Style.textColor;

	property bool hasIcon: true;
	property bool canClose: true;
	property string iconSource: "../../../../" + Style.getIconPath("Icons/AppIcon", Icon.State.On, Icon.Mode.Normal);
	property string closeButtonIconSource: "../../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);

	signal closeButtonClicked(int buttonId);

	Image {
		id: iconDialog;

		anchors.left: topPanelDialogContainer.left;
		anchors.leftMargin: Style.marginM;
		anchors.verticalCenter: topPanelDialogContainer.verticalCenter;

		width: Style.iconSizeM;
		height: width;

		sourceSize.height: height;
		sourceSize.width: width;

		source: topPanelDialogContainer.iconSource;

		visible: topPanelDialogContainer.hasIcon;
	}

	Text {
		id: titleDialog;

		anchors.left: topPanelDialogContainer.hasIcon ? iconDialog.right : parent.left;
		anchors.leftMargin: topPanelDialogContainer.hasIcon ? Style.marginM : 1.5 * Style.marginL;
		anchors.verticalCenter: topPanelDialogContainer.verticalCenter;
		anchors.right: closeButton.left;
		anchors.rightMargin: Style.marginM;

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
		anchors.rightMargin: Style.marginL;

		width: Style.iconSizeM;
		height: width;

		visible: topPanelDialogContainer.canClose;

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
