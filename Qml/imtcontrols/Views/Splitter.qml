import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0;
import imtcontrols 1.0

Rectangle {
	id: splitterContainer;

	color: Style.backgroundColor;

	property int type: 0x1;
	property bool isPressed: false;
	property int leftLimit: 0;
	property int rightLimit: 1000000;

	property bool isClickMode: false;

	signal clicked();

	MouseArea {
		id: splitterMA;

		anchors.fill: parent;

		enabled: parent.enabled
		hoverEnabled: enabled;
		acceptedButtons: Qt.LeftButton;
		cursorShape: splitterContainer.isClickMode ? Qt.PointingHandCursor : Qt.SplitHCursor;

		onPressed: {
			splitterContainer.isPressed = true;
		}
		onReleased: {
			splitterContainer.isPressed = false;
			splitterContainer.clicked();
		}

		onMouseXChanged: {
			if (splitterContainer.type !== 0x1 || splitterContainer.isClickMode){
				return;
			}

			if (splitterContainer.isPressed){
				let newX = splitterContainer.x + mouseX;
				if((newX > splitterContainer.x && newX < splitterContainer.rightLimit)||
						(newX < splitterContainer.x && newX > splitterContainer.leftLimit)){

					splitterContainer.x += mouseX;
				}
			}
		}
	}
}
