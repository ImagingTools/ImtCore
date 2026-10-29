import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

// "Continue with ..." button of an external identity provider: brand logo on the left, label centered.
Rectangle {
	id: root;

	property string providerName: "";
	// "Google", "Apple", "Facebook" or "Generic", as reported by the server.
	property string providerType: "";
	property bool loading: false;

	property string iconName: root.providerType === "Google" ? "Icons/Google" :
							  root.providerType === "Apple" ? "Icons/Apple" :
							  root.providerType === "Facebook" ? "Icons/Facebook" : "Icons/Key";

	signal clicked();

	height: Style.controlHeightL;

	radius: Style.radiusM;
	color: !root.enabled ? Style.backgroundColor : mouseArea.pressed ? Style.buttonPressedColor : mouseArea.containsMouse ? Style.neutralSubtleColor : Style.baseColor;
	border.width: 1;
	border.color: mouseArea.containsMouse && root.enabled ? Style.buttonHoverColor : Style.borderColor;

	Image {
		id: providerIcon;

		anchors.left: parent.left;
		anchors.leftMargin: Style.marginL;
		anchors.verticalCenter: parent.verticalCenter;

		width: Style.iconSizeM;
		height: width;
		sourceSize.width: width;
		sourceSize.height: height;
		opacity: root.enabled ? 1 : 0.5;

		source: "../../../" + Style.getIconPath(root.iconName, Icon.State.On, Icon.Mode.Normal);
	}

	Text {
		id: labelText;

		anchors.verticalCenter: parent.verticalCenter;
		anchors.left: providerIcon.right;
		anchors.leftMargin: Style.marginM;
		anchors.right: parent.right;
		anchors.rightMargin: Style.marginL + Style.iconSizeM + Style.marginM;

		color: root.enabled ? Style.textColor : Style.inactiveTextColor;
		font.family: Style.fontFamilyBold;
		font.pixelSize: Style.fontSizeM;
		horizontalAlignment: Text.AlignHCenter;
		elide: Text.ElideRight;

		text: qsTr("Continue with %1").arg(root.providerName);
	}

	Loading {
		anchors.right: parent.right;
		anchors.rightMargin: Style.marginL;
		anchors.verticalCenter: parent.verticalCenter;

		width: 18;
		height: 18;
		visible: root.loading;
		color: "transparent";
		background.color: "transparent";
		indicatorSize: 14;
	}

	MouseArea {
		id: mouseArea;

		anchors.fill: parent;
		hoverEnabled: true;
		enabled: root.enabled;
		cursorShape: Qt.PointingHandCursor;

		onClicked: {
			root.clicked();
		}
	}
}
