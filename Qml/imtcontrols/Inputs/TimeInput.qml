import QtQuick 2.15
import Acf 1.0
import imtcontrols 1.0

Item {
	id: timeInput;

	width: 84;
	height: 36;

	property int mainMargin: 0;
	property int fontSize: Style.fontSizeNormal;

	property string fontColor: Style.textColor;

	property string placeHolderText: "hh:mm";

	property var timeRegExpFull: /^(([0-1]\d)|(2[0-3])):[0-5]\d$/
	//property var timeRegExp: /^(([0-1]\d)|(2[0-3]))?:{0,1}([0-5]\d)?$/
	property var timeRegExp: /^[0-9:]{0,5}$/

	property bool canShowCurrentTime: false;

	property bool  isError: false;

	property Item tabKeyItem: null//input;

	property alias inputItem: input;

	property string time;

	signal accepted(string value);

	Component.onCompleted:{
		if(canShowCurrentTime){
			showCurrentTime();
		}
	}

	function checkDateFormat(str){
		console.log("Check Date Format")
		timeInput.isError = str.match(timeInput.timeRegExpFull) === null;
		if(timeInput.isError){
			console.log("Wrong time format!")
		}
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
			console.log("Wrong time format!")
			return;
		}

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

	CustomTextField{
		id: input

		anchors.centerIn: parent

		width: parent.width - 2 * parent.mainMargin;
		height: parent.height - 2 * parent.mainMargin;

		textSize: parent.fontSize;
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
				timeInput.checkDateFormat(input.text);
			}
		}
		onAccepted: {
			if(timeInput.checkDateFormat(input.text)){
				timeInput.time = input.text;
				timeInput.accepted(input.text);
			}

		}

		onTextChanged: {
			if(text !== ""){
				isEmpty = false;
				timeInput.isError = false;
			}

		}
	}

	Rectangle{
		id: frame;

		anchors.fill: parent;

		radius: input.radius;
		color: "transparent";
		border.color: timeInput.isError ? Style.errorTextColor : "transparent";
	}

	CustomTooltip{
		id: tooltip;

		text: "Wrong time format!"
	}


	PauseAnimation {
		id: closeTooltipPause;

		duration: 2000
		onFinished: {
			tooltip.hide();
		}
	}
}
