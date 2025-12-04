import QtQuick 2.12
import Acf 1.0

Rectangle {
	id: root;

	property int indicatorSize: 50;
	property alias background: backgroundRec;
	color: "transparent"

	function start(){
		root.visible = width >= square.width && height >= square.height
	}

	function stop(){
		root.visible = false;
	}

	Rectangle {
		id: backgroundRec;

		anchors.fill: parent;

		color: Style.backgroundColor;
	}

	Item { // square
		id: square;

		z: root.z + 1;

		anchors.centerIn: parent
		property double minimum: Math.min(root.width, root.height);
		width: root.indicatorSize;
		height: width;

		Repeater {
			id: repeater;

			model: 8;

			delegate: loadingDelegate
			Component {
				id: loadingDelegate
				Rectangle {
					color: Style.textColor;

					property double b: 0.1;
					property double a: 0.25;

					width: repeater.count == 0 ? square.height : ((b - a) / repeater.count * index + a) * square.height;
					height: width;
					radius: 0.5 * height;

					x: 0.5 * square.width  + 0.5 * (square.width  - width )  * Math.cos(2 * Math.PI / repeater.count * model.index) - 0.5 * width;
					y: 0.5 * square.height - 0.5 * (square.height - height)  * Math.sin(2 * Math.PI / repeater.count * model.index) - 0.5 * height;
				}
			}
		}
	}

	MouseArea {
		anchors.fill: parent;
		hoverEnabled: true;
		preventStealing: true;
		acceptedButtons: Qt.AllButtons

		onWheel: {
			wheel.accepted = true;
		}
		onClicked: {
			mouse.accepted = true;
		}
		onReleased: {}
		onPressAndHold: {}
		onPressed: {}
		onPositionChanged: {}
	}

	Timer {
		interval: 10;
		running: root.visible;
		repeat:  true;

		onTriggered: {
			square.rotation += 2; // degrees
		}
	}
}
