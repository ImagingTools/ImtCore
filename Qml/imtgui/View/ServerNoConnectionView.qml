import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: container;

	color: Style.baseColor;

	property alias text: textNoConnection.text;
	property alias loadingVisible: loading.visible;

	Component.onCompleted: {
		if (loading.visible){
			timer.start()
		}

		textNoConnection.visible = !loading.visible
	}

	Timer {
		id: timer
		interval: 2000
		repeat: false
		onTriggered: {
			textNoConnection.visible = true
		}
	}

	Column {
		id: content;
		anchors.centerIn: parent;
		width: Math.min(parent.width - 2 * Style.marginXXL, Style.sizeHintL);
		spacing: Style.marginXL;

		Item {
			width: parent.width;
			height: Math.max(loading.height, errorIcon.height);

			Loading {
				id: loading;
				anchors.centerIn: parent;
				width: Style.sizeHintBXS;
				height: width;
				visible: false;
				background.color: Style.baseColor;
			}

			Image {
				id: errorIcon;
				anchors.centerIn: parent;
				visible: !loading.visible && textNoConnection.visible;
				width: Style.iconSizeXXL;
				height: width;
				source: "qrc:/" + Style.getIconPath("Icons/Error", Icon.State.On, Icon.Mode.Normal);
				sourceSize.width: width;
				sourceSize.height: height;
				opacity: Style.opacityLow;
			}
		}

		Text {
			id: textNoConnection;
			width: parent.width;
			anchors.horizontalCenter: parent.horizontalCenter;
			horizontalAlignment: Text.AlignHCenter;
			color: Style.titleColor;
			font.family: Style.fontFamilyBold;
			font.pixelSize: Style.fontSizeXL;
			wrapMode: Text.Wrap;
			visible: false;
		}
	}

	MouseArea {
		anchors.fill: parent;
		hoverEnabled: true
		onWheel: {}
		onClicked: {}
	}
}
