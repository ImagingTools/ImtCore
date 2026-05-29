import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0

Rectangle {
	id: root;

	property string text: qsTr("Sign in with OpenID")
	property bool loading: false

	signal clicked()

	width: Style.buttonWidthXXL;
	height: Style.controlHeightM;

	radius: Style.radiusS;
	color: root.enabled ? (mouseArea.containsMouse ? Style.hoverColor : Style.accentColor) : Style.disabledColor;

	border.width: 1
	border.color: Style.borderColor

	opacity: root.enabled ? 1.0 : 0.6

	Row {
		anchors.centerIn: parent;
		spacing: Style.marginS;

		// OpenID icon placeholder
		Text {
			anchors.verticalCenter: parent.verticalCenter;
			text: "\u{1F511}"; // Key emoji as placeholder
			font.pixelSize: Style.fontSizeL;
			visible: !root.loading;
		}

		Text {
			anchors.verticalCenter: parent.verticalCenter;
			text: root.loading ? qsTr("Connecting...") : root.text;
			color: Style.buttonTextColor;
			font.family: Style.fontFamilyBold;
			font.pixelSize: Style.fontSizeM;
		}
	}

	MouseArea {
		id: mouseArea;

		anchors.fill: parent;
		cursorShape: root.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor;
		hoverEnabled: true;
		enabled: root.enabled && !root.loading;

		onClicked: {
			root.clicked();
		}
	}
}
