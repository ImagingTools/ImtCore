import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

ControlBase {
	id: baseButton;
	objectName: text.replace(/\s/g, '') + "Button"

	decorator: Style.buttonDecorator

	property string text: ""
	property string iconSource: ""
	property string tooltipText: "";
	property color textColor: Style.textColor;

	property bool down: false
	property bool hoverEnabled: true
	property bool hovered: enabled && hoverEnabled ? ma.containsMouse : false;
	property bool checkable: false;
	property bool checked: false;
	property bool enabled: true;
	property bool flat: false;
	property bool highlighted: false;

	// Auto-repeat support – mirrors QtQuick.Controls.AbstractButton
	property bool autoRepeat: false;
	property int autoRepeatDelay: 300;
	property int autoRepeatInterval: 100;

	property bool isMenuButton: false;

	property int focusPolicy: Qt.WheelFocus;
	property int focusReason: -1;

	property alias mouseArea: ma;
	property IconProperties icon: buttonIcon;
	property FontProperties font: buttonFont;

	signal clicked();
	signal doubleClicked();
	signal pressed();
	signal released();
	signal entered(real mouseX, real mouseY);
	signal exited(real mouseX, real mouseY);
	signal positionChanged(real mouseX, real mouseY);
	signal toggled();
	signal closeTooltip();
	signal pressAndHold();
	signal canceled();

	QtObject {
		id: _private
		function onClicked(){
			// baseButton.focus = true;
			// baseButton.forceActiveFocus();
			if (baseButton.checkable){
				baseButton.checked = !baseButton.checked
				baseButton.toggled()
			}
			baseButton.closeTooltip();
			baseButton.clicked();
		}
	}

	IconProperties{
		id: buttonIcon;
	}

	FontProperties{
		id: buttonFont;
	}

	onDecoratorChanged: {
		if(focus){
			forceActiveFocus()
		}
	}

	Keys.onPressed: {
		if (event.key === Qt.Key_Return || event.key === Qt.Key_Space){
			event.accepted = true;
			_private.onClicked()
		}
	}

	// Accessibility – mirrors QtQuick.Controls.AbstractButton


	// Auto-repeat timer
	Timer {
		id: autoRepeatTimer
		repeat: true
		onTriggered: {
			if (baseButton.autoRepeat && baseButton.down)
				_private.onClicked()
		}
	}

	MouseArea{
		id: ma;
		objectName: "MouseArea"

		anchors.fill: parent;

		z: 10;

		enabled: baseButton.enabled;
		hoverEnabled: enabled;
		cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
		pressAndHoldInterval: baseButton.autoRepeatDelay

		onClicked: {
			if (baseButton.enabled){
				_private.onClicked()
			}
		}

		onDoubleClicked: {
			baseButton.doubleClicked();
		}

		onPressed: {
			baseButton.down = true
			baseButton.pressed();
			if (baseButton.autoRepeat) {
				// Start the hold-detection timer; first auto-repeat fires
				// in onPressAndHold after autoRepeatDelay ms.
				autoRepeatTimer.interval = baseButton.autoRepeatInterval
				autoRepeatTimer.stop()
			}
		}

		onReleased: {
			baseButton.down = false
			autoRepeatTimer.stop()
			baseButton.released();
		}

		onPressAndHold: {
			baseButton.pressAndHold();
			if (baseButton.autoRepeat) {
				// First auto-repeat click now, then repeat at autoRepeatInterval.
				_private.onClicked()
				autoRepeatTimer.interval = baseButton.autoRepeatInterval
				autoRepeatTimer.restart()
			}
		}

		onEntered: {
			if (baseButton){
				baseButton.entered(mouseX, mouseY);
			}
		}

		onExited: {
			if (baseButton){
				baseButton.exited(mouseX, mouseY);
				baseButton.down = false
			}
			autoRepeatTimer.stop()
		}

		onPositionChanged: {
			if (baseButton){
				baseButton.positionChanged(mouse.x, mouse.y);
			}
		}

		onCanceled: {
			baseButton.down = false
			autoRepeatTimer.stop()
			baseButton.canceled()
		}
	}
}
