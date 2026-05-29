import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0

Column {
	id: root;

	property var providers: []

	signal providerSelected(string providerId)

	spacing: Style.marginS;

	Repeater {
		model: root.providers

		delegate: Rectangle {
			id: providerButton;

			width: Style.buttonWidthXXL;
			height: Style.controlHeightM;
			anchors.horizontalCenter: parent.horizontalCenter;

			radius: Style.radiusS;
			color: providerMouseArea.containsMouse ? Style.hoverColor : getProviderColor(modelData.providerType);
			border.width: 1
			border.color: Style.borderColor

			Row {
				anchors.centerIn: parent;
				spacing: Style.marginS;

				Text {
					anchors.verticalCenter: parent.verticalCenter;
					text: getProviderIcon(modelData.providerType);
					font.pixelSize: Style.fontSizeL;
				}

				Text {
					anchors.verticalCenter: parent.verticalCenter;
					text: qsTr("Sign in with %1").arg(modelData.displayName);
					color: getProviderTextColor(modelData.providerType);
					font.family: Style.fontFamilyBold;
					font.pixelSize: Style.fontSizeM;
				}
			}

			MouseArea {
				id: providerMouseArea;

				anchors.fill: parent;
				cursorShape: Qt.PointingHandCursor;
				hoverEnabled: true;

				onClicked: {
					root.providerSelected(modelData.providerId);
				}
			}
		}
	}

	function getProviderColor(providerType) {
		switch (providerType) {
			case "GOOGLE":
				return "#FFFFFF";
			case "FACEBOOK":
				return "#1877F2";
			case "APPLE":
				return "#000000";
			default:
				return Style.accentColor;
		}
	}

	function getProviderTextColor(providerType) {
		switch (providerType) {
			case "GOOGLE":
				return "#757575";
			case "FACEBOOK":
				return "#FFFFFF";
			case "APPLE":
				return "#FFFFFF";
			default:
				return Style.buttonTextColor;
		}
	}

	function getProviderIcon(providerType) {
		switch (providerType) {
			case "GOOGLE":
				return "G";
			case "FACEBOOK":
				return "f";
			case "APPLE":
				return "\uF8FF";
			default:
				return "\u{1F511}";
		}
	}
}
