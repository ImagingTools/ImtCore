import QtQuick 2.15
import QtGraphicalEffects 1.12
import Qt5Compat.GraphicalEffects

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0


PopupView {
	id: timeInputDialog;

	width: Style.sizeHintBXS;
	height: 90;

	property int radius: Style.radiusL;
	property string color: Style.baseColor;

	property int hours: 0;
	property int minutes: 0;

	property string hoursStr: timeInputDialog.hours < 10 ? "0" + String(timeInputDialog.hours) : String(timeInputDialog.hours);
	property string minutesStr: timeInputDialog.minutes < 10 ? "0" + String(timeInputDialog.minutes) : String(timeInputDialog.minutes);

	property string time: hoursStr + ":" + minutesStr;

	property int pauseAnimDuration: 400;

	signal accepted(string time);

	Component.onDestruction: {
		accepted(time);
	}

	function increaseHourse(){
		if(timeInputDialog.hours < 23){
			timeInputDialog.hours++;
		}
		else{
			timeInputDialog.hours = 0;
		}
	}
	function decreaseHourse(){
		if(timeInputDialog.hours > 0){
			timeInputDialog.hours--;
		}
		else {
			timeInputDialog.hours = 23;
		}
	}
	function increaseMinutes(){
		if(timeInputDialog.minutes < 59){
			timeInputDialog.minutes++;
		}
		else {
			timeInputDialog.minutes = 0;
		}
	}
	function decreaseMinutes(){
		if(timeInputDialog.minutes > 0){
			timeInputDialog.minutes--;
		}
		else {
			timeInputDialog.minutes = 59;
		}
	}

	DropShadow {
		id: dropShadow;

		anchors.fill: forShadowRec;

		visible: forShadowRec.visible;
		horizontalOffset: 2;
		verticalOffset: 2;

		radius: forShadowRec.radius;
		color: Style.shadowColor;

		source: forShadowRec;
	}

	Rectangle{
		id: forShadowRec;

		anchors.fill: parent;
		radius: parent.radius;
		color: parent.color;
	}

	Rectangle{
		id: body;

		anchors.fill: parent;

		radius: timeInputDialog.radius;
		color: parent.color;
		border.color: Style.borderColor;

		property int buttonWidth: Style.buttonWidthS;
		property int buttonHeight: Style.buttonHeightXXS;
		property int fontSize: Style.fontSizeM;

		Item{
			id: hoursColumn;

			anchors.left: parent.left;
			anchors.leftMargin: Style.marginM;

			width: body.buttonWidth;
			height: parent.height;

			Button{
				id: hoursButtonUp;

				anchors.horizontalCenter: parent.horizontalCenter;
				anchors.top: parent.top;
				anchors.topMargin: Style.marginXS;

				width: body.buttonWidth;
				height: body.buttonHeight;

				iconSource: "../../../" + Style.getIconPath("Icons/Up", Icon.State.On, Icon.Mode.Selected);
				decorator: Component{IconButtonDecorator{}}

				onPressed: {
					hoursUpAnim.canRepeat = true;
					hoursUpAnim.start();
				}
				onReleased: {
					hoursUpAnim.canRepeat = false;
				}
			}

			Item{
				anchors.top: hoursButtonUp.bottom;
				anchors.bottom: hoursButtonDown.top;

				width: parent.width;

				Text{
					id: hoursText;

					anchors.centerIn: parent;

					font.family: Style.fontFamily;
					font.pixelSize: body.fontSize;
					color: Style.textColor;

					text: timeInputDialog.hoursStr;

				}
				MouseArea{
					anchors.fill: parent;

					onWheel: {
						if(wheel.angleDelta.y > 0){//up
							timeInputDialog.increaseHourse();
						}
						else{
							timeInputDialog.decreaseHourse();
						}
					}
				}
			}

			Button{
				id: hoursButtonDown;

				anchors.horizontalCenter: parent.horizontalCenter;
				anchors.bottom: parent.bottom;
				anchors.bottomMargin: Style.marginXS;

				width: body.buttonWidth;
				height: body.buttonHeight;

				iconSource: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Selected);
				decorator: Component{IconButtonDecorator{}}

				onPressed: {
					hoursDownAnim.canRepeat = true;
					hoursDownAnim.start();
				}
				onReleased: {
					hoursDownAnim.canRepeat = false;
				}

			}
		}//hoursColumn

		Item{
			id: minutesColumn;

			anchors.right: parent.right;
			anchors.verticalCenter: parent.verticalCenter;
			anchors.rightMargin: Style.marginM;

			width: body.buttonWidth;
			height: parent.height


			Button{
				id: minutesButtonUp;

				anchors.horizontalCenter: parent.horizontalCenter;
				anchors.top: parent.top;
				anchors.topMargin: Style.marginXS;

				width: body.buttonWidth;
				height: body.buttonHeight;

				iconSource: "../../../" + Style.getIconPath("Icons/Up", Icon.State.On, Icon.Mode.Selected);
				decorator: Component{IconButtonDecorator{}}

				onPressed: {
					minutesUpAnim.canRepeat = true;
					minutesUpAnim.start();
				}
				onReleased: {
					minutesUpAnim.canRepeat = false;
				}
			}

			Item{
				anchors.top: minutesButtonUp.bottom;
				anchors.bottom: minutesButtonDown.top;
				width: parent.width;

				Text{
					id: minutesText;

					anchors.centerIn: parent;

					font.family: Style.fontFamily;
					font.pixelSize: body.fontSize;
					color: Style.textColor;

					text: timeInputDialog.minutesStr;

				}
				MouseArea{
					anchors.fill: parent;
					onWheel: {
						if(wheel.angleDelta.y > 0){//up
							timeInputDialog.increaseMinutes();
						}
						else{
							timeInputDialog.decreaseMinutes();
						}
					}
				}
			}

			Button{
				id: minutesButtonDown;

				anchors.horizontalCenter: parent.horizontalCenter;
				anchors.bottom: parent.bottom;
				anchors.bottomMargin: Style.marginXS;

				width: body.buttonWidth;
				height: body.buttonHeight;

				iconSource: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Selected);
				decorator: Component{IconButtonDecorator{}}

				onPressed: {
					minutesDownAnim.canRepeat = true;
					minutesDownAnim.start();
				}
				onReleased: {
					minutesDownAnim.canRepeat = false;
				}
			}
		}//minutesColumn

		Text{
			anchors.centerIn:  parent;

			font.family: Style.fontFamily;
			font.pixelSize: body.fontSize;
			color: Style.textColor;

			text: ":"
		}
	}


	PauseAnimation {
		id: hoursUpAnim;

		duration: timeInputDialog.pauseAnimDuration;
		property bool canRepeat:false;

		onFinished: {
			timeInputDialog.increaseHourse();
			if(canRepeat){
				start();
			}
		}
	}
	PauseAnimation {
		id: hoursDownAnim;

		duration: timeInputDialog.pauseAnimDuration;
		property bool canRepeat:false;

		onFinished: {
			timeInputDialog.decreaseHourse();
			if(canRepeat){
				start();
			}
		}
	}
	PauseAnimation {
		id: minutesUpAnim;

		duration: timeInputDialog.pauseAnimDuration;
		property bool canRepeat:false;

		onFinished: {
			timeInputDialog.increaseMinutes();
			if(canRepeat){
				start();
			}
		}
	}
	PauseAnimation {
		id: minutesDownAnim;

		duration: timeInputDialog.pauseAnimDuration;
		property bool canRepeat:false;

		onFinished: {
			timeInputDialog.decreaseMinutes();
			if(canRepeat){
				start();
			}
		}
	}

	Shortcut {
		sequence: "Escape";
		enabled: true;
		onActivated: {
			ModalDialogManager.closeDialog();
		}
	}
}
