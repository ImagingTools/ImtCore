import QtQuick 2.15
import Acf 1.0
import imtcontrols 1.0

Item {
	id: timeInput;

	width: 64;
	height: column.height;

	property int inputHeight: 30;
	property int fontSize: Style.fontSizeNormal;

	property string fontColor: Style.textColor;
	property alias radius: input.radius;
	property alias color: input.color;

	property int timeInputDialogWidth: 100;
	property int timeInputDialogHeight: 90;

	property string placeHolderText: "hh:mm";

	property var timeRegExpFull: /^(([0-1]\d)|(2[0-3])):[0-5]\d$/
	property var timeRegExp: /^[0-9:]{0,5}$/

	property bool canShowCurrentTime: false;
	property bool hasTitle: true;

	property bool  isError: false;

	property Item tabKeyItem: null//input;

	property alias inputItem: input;

	property string selectedTime: "00:00";

	signal timeChanged(string value);
	signal dialogClosed();

	Component.onCompleted:{
		if(canShowCurrentTime && selectedTime == "00:00"){
			showCurrentTime();
		}
	}

	function checkDateFormat(str){
		//console.log("Check Date Format")
		str = correctTimeFormat(str);
		timeInput.isError = str.match(timeInput.timeRegExpFull) === null;

		if(timeInput.isError){
			let point = timeInput.mapToItem(null, 0, -8 - tooltip.componentHeight)
			tooltip.openTooltipWithCoord(point.x,point.y)
			closeTooltipPause.restart();
		}
		return !timeInput.isError;
	}

	function correctTimeFormat(str){
		let str_ = str;
		let reg = /^\d{0,2}:\d{0,2}$/
		if(str.match(reg) === null){
			return str;
		}
		let arr = str.split(":");
		let hours = arr[0];
		let minutes = arr[1];
		if(String(hours).length == 1){
			hours = "0" + hours;
		}
		if(String(hours).length == 0){
			hours = "00";
		}
		if(String(minutes).length == 1){
			minutes = "0" + minutes;
		}
		if(String(minutes).length == 0){
			minutes = "00";
		}
		str_ = hours + ":" + minutes;
		input.text = str_;
		return str_;
	}

	function setTime(str){
		console.log("Set time", str)
		if(str.match(timeInput.timeRegExpFull) === null){
			//console.log("Wrong time format!")
			return false;
		}

		timeInput.selectedTime = str;
		input.text = str
		return true;
	}

	function showCurrentTime(){
		// console.log("showCurrentTime")
		let date = new Date();

		let hours = date.getHours();
		let minutes = date.getMinutes();
		if(String(hours).length == 1){
			hours = "0" + hours;
		}
		if(String(minutes).length == 1){
			minutes = "0" + minutes;
		}

		let time  = hours + ":" + minutes;
		console.log("showCurrentTime:: ", time)
		setTime(time);
		timeChanged(time);
	}

	function getHours(){
		let hours
		if(selectedTime !== "" && selectedTime.match(timeInput.timeRegExpFull) !== null){
			hours = selectedTime.slice(0,2);
			if(hours[0] == "0"){
				hours = hours[1]
			}
			return hours;
		}
		return 0;
	}

	function getMinutes(){
		let minutes
		if(selectedTime !== "" && selectedTime.match(timeInput.timeRegExpFull) !== null){
			minutes = selectedTime.slice(3);
			if(minutes[0] == "0"){
				minutes = minutes[1]
			}
			return minutes;
		}
		return 0;
	}

	Column{
		id: column;

		spacing: Style.size_smallMargin;

		Text{
			id: titleText;

			font.family: Style.fontFamily;
			font.pixelSize: Style.fontSizeNormal;
			color: Style.textColor;

			visible: timeInput.hasTitle;

			text: qsTr("Time:");
		}

		CustomTextField{
			id: input

			width: timeInput.width;
			height: timeInput.inputHeight;

			textSize: timeInput.fontSize;
			fontColor: timeInput.fontColor;
			borderColor: Style.iconColorOnSelected;

			KeyNavigation.tab: timeInput.tabKeyItem;

			placeHolderText: timeInput.placeHolderText;
			placeHolderTextSize: timeInput.fontSize-2;

			textInputValidator : RegularExpressionValidator { regularExpression: timeInput.timeRegExp }

			property bool isEmpty: false;

			onVisibleChanged: {
				if(text == ""){
					isEmpty = true;
				}
			}

			onFocusChanged: {
				//console.log("FOCUS:: ", focus)
				if(!focus){
					if(timeInput.checkDateFormat(input.text)){
						timeInput.selectedTime = input.text;
						timeInput.timeChanged(input.text);
					}
					ModalDialogManager.closeByComp(timeInputDialogComp);
				}
				else {
					let hours = timeInput.getHours();
					let minutes = timeInput.getMinutes();

					let point = input.mapToItem(null, 0, input.height + 2);
					let y_ = point.y;
					if(y_ + timeInput.timeInputDialogHeight > ModalDialogManager.activeView.height){
						y_ = point.y - (input.height + 2) - timeInput.timeInputDialogHeight - 2;
					}
					let parameters = {"x": point.x, "y":y_, "hours": hours, "minutes": minutes};

					ModalDialogManager.openDialog(timeInputDialogComp, parameters);
				}
			}
			onAccepted: {
				if(timeInput.checkDateFormat(input.text)){
					timeInput.selectedTime = input.text;
					timeInput.timeChanged(input.text);
					timeInput.tabKeyItem.forceActiveFocus();
					//ModalDialogManager.closeByComp(timeInputDialogComp);
				}

			}

			onTextChanged: {
				if(text !== ""){
					isEmpty = false;
					timeInput.isError = false;
				}

			}

			Rectangle{
				id: frame;

				anchors.fill: parent;

				radius: input.radius;
				color: "transparent";
				border.color: timeInput.isError ? Style.errorTextColor : "transparent";
			}
		}
	}



	CustomTooltip{
		id: tooltip;

		text: qsTr("Wrong time format!")

		fontPixelSize: Style.fontSizeNormal
	}


	PauseAnimation {
		id: closeTooltipPause;

		duration: 2000
		onFinished: {
			tooltip.hide();
		}
	}

	Component{
		id: timeInputDialogComp;
		TimeInputDialog{
			width: timeInput.timeInputDialogWidth;
			height: timeInput.timeInputDialogHeight;
			onTimeChanged: {
				if(time !== timeInput.selectedTime){
					timeInput.setTime(time)
				}
			}
			Component.onDestruction:{
				timeInput.dialogClosed();
			}
		}
	}


}
