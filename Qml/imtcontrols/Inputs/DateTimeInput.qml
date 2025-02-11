import QtQuick 2.15
import Acf 1.0
import imtcontrols 1.0

Row{
	id: dateTimeInput;

	height: Math.max(dateInput.height, timeInput.height);

	spacing: 16;

	property bool canShowCurrentTime: true;
	property bool hasTitle: true;
	property string color: Style.baseColor;

	property var selectedDate;

	property string dateStr: "";
	property string timeStr: "";
	property string dateTimeStr: dateStr + ":" + timeStr;

	signal dateChanged(string value)

	Component.onCompleted: {
		selectedDate = new Date();
	}

	function setDateTime(date){
		setDate(date);
		setTime(date);
	}

	function setDate(date){
		dateTimeInput.dateStr = Functions.dateToStr(date,"dd.MM.yyyy");
		dateInput.setDate(date);

		dateTimeInput.selectedDate.setFullYear(date.getFullYear())
		dateTimeInput.selectedDate.setMonth(date.getMonth())
		dateTimeInput.selectedDate.setDate(date.getDate())

		signalPause.restart();

	}

	function setTime(date){
		let hours = date.getHours();
		let minutes = date.getMinutes();
		if(Number(hours) < 10){
			hours = "0" + hours;
		}
		if(Number(minutes) < 10){
			minutes = "0" + minutes;
		}
		dateTimeInput.timeStr = hours + ":" + minutes
		timeInput.setTime(dateTimeInput.timeStr);

		dateTimeInput.selectedDate.setHours(date.getHours())
		dateTimeInput.selectedDate.setMinutes(date.getMinutes())

		signalPause.restart();
	}


	function setDateTimeAsString(str){
	}

	function setDateAsString(str){
	}

	function setTimeAsString(str){
	}


	DateInput{
		id: dateInput;

		anchors.bottom: parent.bottom;

		canShowCurrentDate: dateTimeInput.canShowCurrentTime;
		hasTitle: dateTimeInput.hasTitle;
		color: dateTimeInput.color;

		tabKeyItem: timeInput.inputItem;
		onDateChanged: {

		}
	}

	TimeInput{
		id: timeInput;

		anchors.bottom: parent.bottom;

		canShowCurrentTime: dateTimeInput.canShowCurrentTime;
		hasTitle: dateTimeInput.hasTitle;
		color: dateTimeInput.color;

		tabKeyItem: dateInput.inputItem;
		onTimeChanged: {

		}
	}

	PauseAnimation {
		id: signalPause;

		duration: 200;
		onFinished: {
			dateChanged(dateTimeInput.dateTimeStr);
		}
	}
}
