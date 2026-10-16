import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: topPanelDialogContainer;
	objectName: "TopPanel"

	width: Style.sizeHintXS;
	height: Style.controlHeightL;
	radius: Style.radiusM;

	color: Style.baseColor;

	property string title;
	property int titleFontSize: Style.fontSizeL;
	property string titleFontFamily: Style.fontFamilyBold;
	property color titleTextColor: Style.textColor;

	property bool hasIcon: true;
	property bool canClose: true;
	// The installation's own logo, falling back to the generic mark. It used to
	// ask for the generic one outright, so a deployment that had named a logo
	// never saw it here.
	property string iconSource: "qrc:/" + Style.getLogoIconPath(Icon.State.On, Icon.Mode.Normal);
	property string closeButtonIconSource: "qrc:/" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);

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
		font.bold: true;

		text: topPanelDialogContainer.title;

		elide: Text.ElideRight;
	}

	Button {
		id: closeButton;
		objectName: "CloseButton"

		anchors.right: topPanelDialogContainer.right;
		anchors.verticalCenter: topPanelDialogContainer.verticalCenter;
		anchors.rightMargin: Style.marginL;

		width: Style.buttonWidthL;
		height: width;

		visible: topPanelDialogContainer.canClose;

		iconSource: topPanelDialogContainer.closeButtonIconSource;

		onClicked: {
			topPanelDialogContainer.closeButtonClicked(Enums.cancel);
		}

		decorator: Component {
			id: buttonDecorator;

			ButtonDecorator {
				color: closeButton.hovered ? Style.toolButtonHoverColor : "transparent";
				border.width: 0;
				radius: Style.buttonRadius;
				icon.width: Style.iconSizeS;
				icon.height: Style.iconSizeS;
			}
		}
	}
}
