import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: container;
	
	color: Style.backgroundColor;
	
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
		width: parent.width - 100;
		spacing: Style.marginM;
		
		Text {
			id: textNoConnection;
			anchors.horizontalCenter: parent.horizontalCenter;
			horizontalAlignment: Text.AlignHCenter;
			color: Style.textColor;
			font.pixelSize: Style.fontSizeXXL;
			font.family: Style.fontFamily;
			wrapMode: Text.Wrap;
			visible: false
		}
		
		Loading {
			id: loading;
			anchors.horizontalCenter: parent.horizontalCenter;
			width: Style.sizeHintBXS;
			height: width;
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
