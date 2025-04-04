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
	property string dateTimeStr: dateStr + " " + timeStr;

	property var dateTimeRegExp: /^\d{2}\.\d{2}\.\d{4}\s\d{2}:\d{2}$/

	property int calendarWidth: 350;
	property int calendarHeight: 350;
	property int timeInputDialogWidth: 100;
	property int timeInputDialogHeight: 90;
	property int fontSize: Style.fontSizeNormal;

	property alias dateInputWidth: dateInput.width;
	property alias timeInputWidth: timeInput.width;

	//time format: "dd.MM.yyyy hh:mm"

	signal dateChanged(string value)

	Component.onCompleted: {
		selectedDate = new Date();
	}

	function setDateTime(datetime){
		console.log("setDateTime", datetime)
		setDate(datetime);
		signalPause.stop()
		setTime(datetime);
		selectedDate = datetime
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
		if(str.match(dateTimeInput.dateTimeRegExp) !== null){
			let date_= str.slice(0,10);
			let time_ = str.slice(11);
			setDateAsString(date_);
			setTimeAsString(time_);
		}
	}

	function setDateAsString(str){
		if(dateInput.checkDateFormat(str)){
			dateStr = str;

			let day = str.slice(0, 2)
			let month = str.slice(3,5)
			let year = str.slice(6);
			if(day[0] == "0"){
				day = day[1]
			}
			if(month[0] == "0"){
				month = month[1]
			}
			month -=1

			dateTimeInput.selectedDate.setFullYear(year);
			dateTimeInput.selectedDate.setMonth(month);
			dateTimeInput.selectedDate.setDate(day);

			dateInput.setDateAsString(str);
			signalPause.restart();
		}

	}

	function setTimeAsString(str){
		if(timeInput.checkDateFormat(str)){
			timeStr = str;

			let hours = str.slice(0, 2)
			let minutes = str.slice(3);
			if(hours[0] == "0"){
				hours = hours[1];
			}
			if(minutes[0] == "0"){
				minutes = minutes[1];
			}
			dateTimeInput.selectedDate.setHours(hours);
			dateTimeInput.selectedDate.setMinutes(minutes);

			timeInput.setTime(str);
			signalPause.restart();
		}

	}


	DateInput{
		id: dateInput;

		anchors.bottom: parent.bottom;

		canShowCurrentDate: dateTimeInput.canShowCurrentTime;
		hasTitle: dateTimeInput.hasTitle;
		color: dateTimeInput.color;
		fontSize: dateTimeInput.fontSize

		calendarWidth:  dateTimeInput.calendarWidth
		calendarHeight: dateTimeInput.calendarHeight

		tabKeyItem: timeInput.inputItem;
		onCalendarButtonClicked: {
			forFocus.forceActiveFocus();
		}
		onDateChanged: {
			console.log("onDateChanged", value)
			if(value !== dateTimeInput.dateStr){
				dateTimeInput.dateStr = value;

				dateTimeInput.selectedDate.setFullYear(dateInput.selectedDate.getFullYear())
				dateTimeInput.selectedDate.setMonth(dateInput.selectedDate.getMonth())
				dateTimeInput.selectedDate.setDate(dateInput.selectedDate.getDate())

				signalPause.restart();
			}

		}
	}

	TimeInput{
		id: timeInput;

		anchors.bottom: parent.bottom;

		canShowCurrentTime: dateTimeInput.canShowCurrentTime;
		hasTitle: dateTimeInput.hasTitle;
		color: dateTimeInput.color;
		fontSize: dateTimeInput.fontSize

		timeInputDialogWidth: dateTimeInput.timeInputDialogWidth;
		timeInputDialogHeight:  dateTimeInput.timeInputDialogHeight;

		tabKeyItem: forFocus;
		onTimeChanged: {
			//console.log("onTimeChanged", value)
			if(value !== dateTimeInput.timeStr){
				dateTimeInput.timeStr = value;

				let hours = value.slice(0,2)
				let minutes = value.slice(3);
				if(hours[0] == "0"){
					hours = hours[1];
				}
				if(minutes[0] == "0"){
					minutes = minutes[1];
				}

				dateTimeInput.selectedDate.setHours(hours)
				dateTimeInput.selectedDate.setMinutes(minutes)

				signalPause.restart();
			}
		}

	}

	Item{id: forFocus}

	PauseAnimation {
		id: signalPause;

		duration: 200;
		onFinished: {
			dateTimeInput.dateChanged(dateTimeInput.dateStr + " " + dateTimeInput.timeStr);
		}
	}
}
