import QtQuick 2.15
import Acf 1.0
import imtcontrols 1.0

Item {
	id: timeInput;

	width: 84;
	height: column.height;

	property int inputHeight: 36;
	property int fontSize: Style.fontSizeNormal;

	property string fontColor: Style.textColor;
	property alias radius: input.radius;
	property alias color: input.color;

	property string placeHolderText: "hh:mm";

	property var timeRegExpFull: /^(([0-1]\d)|(2[0-3])):[0-5]\d$/
	//property var timeRegExp: /^(([0-1]\d)|(2[0-3]))?:{0,1}([0-5]\d)?$/
	property var timeRegExp: /^[0-9:]{0,5}$/

	property bool canShowCurrentTime: false;
	property bool hasTitle: true;

	property bool  isError: false;

	property Item tabKeyItem: null//input;

	property alias inputItem: input;

	property string selectedTime;

	signal timeChanged(string value);

	Component.onCompleted:{
		if(canShowCurrentTime){
			showCurrentTime();
		}
	}

	function checkDateFormat(str){
		//console.log("Check Date Format")
		timeInput.isError = str.match(timeInput.timeRegExpFull) === null;

		if(timeInput.isError){
			let point = timeInput.mapToItem(null, 0, -8 - tooltip.componentHeight)
			tooltip.openTooltipWithCoord(point.x,point.y)
			closeTooltipPause.restart();
		}
		return !timeInput.isError;
	}

	function setTime(str){
		console.log("Set time")
		if(str.match(timeInput.timeRegExpFull) === null){
			//console.log("Wrong time format!")
			return;
		}

		timeInput.selectedTime = str;
		input.text = str
	}

	function showCurrentTime(){
		console.log("showCurrentTime")
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
		//console.log("showCurrentTime:: ", time)
		setTime(time);
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
			placeHolderTextSize: fontSize-2;

			textInputValidator : RegularExpressionValidator { regularExpression: timeInput.timeRegExp }

			property bool isEmpty: false;

			onVisibleChanged: {
				if(text == ""){
					isEmpty = true;
				}
			}

			onFocusChanged: {
				if(!focus){
					if(timeInput.checkDateFormat(input.text)){
						timeInput.timeChanged(input.text);
					}

				}
			}
			onAccepted: {
				if(timeInput.checkDateFormat(input.text)){
					timeInput.selectedTime = input.text;
					timeInput.timeChanged(input.text);
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
}
