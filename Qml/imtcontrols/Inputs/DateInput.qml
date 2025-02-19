import QtQuick 2.15
import Acf 1.0
import imtcontrols 1.0

Item {
	id: dateInput;

	width: 160;
	height: column.height;

	property int inputHeight: 36;
	property int fontSize: Style.fontSizeNormal;

	property string fontColor: Style.textColor;
	property alias radius: input.radius;
	property alias color: input.color;

	property string placeHolderText: "dd.MM.yyyy";

	property var dateRegDay: /^([0-3]\d)$/
	property var dateRegMonth: /^((0[1-9])|(1[0-2]))$/
	//property var dateRegExpFull: /^(([0-3]\d)\.([0-1])\d)\.\d{4}$/
	property var dateRegExpFull: /^\d{2}\.\d{2}\.\d{4}$/
	property var dateRegExp: /^[0-9\.]{0,10}$/

	property bool canShowCurrentDate: false;
	property bool hasTitle: true;

	property bool  isError: false;
	property bool  isMonthError: false;
	property bool  isDayError: false;

	property string errorString: qsTr("Wrong date format!");
	property string errorStringMonth: qsTr("Invalid month value!");
	property string errorStringDay: qsTr("Invalid day value!");

	property Item tabKeyItem: null;

	property alias inputItem: input;

	property string dateStr;
	property var selectedDate;

	signal dateChanged(string value);

	Component.onCompleted:{
		selectedDate = new Date();
		if(canShowCurrentDate){
			showCurrentDate();
		}
	}

	function setDate(date){
		dateInput.selectedDate = date;
		dateInput.dateStr = Functions.dateToStr(date,"dd.MM.yyyy");
		input.text = dateInput.dateStr;
		dateInput.dateChanged(dateInput.dateStr);
	}

	function setDateAsString(str){
		//console.log("Set date", str)
		str = correctDateFormat(str);
		if(!dateInput.checkDateFormat(str)){
			return false;
		}
		if(input.text !== str){
			input.text = str
		}

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

		dateInput.dateStr = str;
		dateInput.selectedDate.setFullYear(year);
		dateInput.selectedDate.setMonth(month);
		dateInput.selectedDate.setDate(day);

		dateInput.dateChanged(dateInput.dateStr);

		return true;
	}

	function showCurrentDate(){
		//console.log("showCurrentDate")
		let date = new Date();

		let day = date.getDate();
		let month = date.getMonth() +1;
		let year = date.getFullYear();
		if(String(day).length == 1){
			day = "0" + day;
		}
		if(String(month).length == 1){
			month = "0" + month;
		}

		let date_  = day + "." + month + "." + year;
		dateInput.setDateAsString(date_);
	}

	function correctDateFormat(str){
		let str_ = str;
		let reg = /^\d{1,2}\.\d{1,2}\.\d{1,4}$/
		if(str.match(reg) === null){
			return str;
		}
		let arr = str.split(".");
		let day = arr[0];
		let month = arr[1];
		let year  = arr[2];
		if(String(day).length == 1){
			day = "0" + day;
		}
		if(String(month).length == 1){
			month = "0" + month;
		}
		if(String(year).length == 1){
			year = "200" + year;
		}
		else if(String(year).length == 2){
			year = "20" + year;
		}
		else if(String(year).length == 3){
			year = "2" + year;
		}

		str_ = day + "." + month + "." + year;
		input.text = str_;
		return str_;
	}

	function checkDateFormat(str){
		//console.log("Check Date Format")
		dateInput.isError = false;
		dateInput.isMonthError = false;
		dateInput.isDayError = false;

		let ok = false;

		let mainRegErr = str.match(dateInput.dateRegExpFull) === null;
		if(mainRegErr){
			dateInput.isError = true;
		}
		else {
			let day = str.slice(0, 2)
			let month = str.slice(3,5)
			let year = str.slice(6);

			let monthErr = !dateInput.checkMonth(month);
			if(monthErr){
				dateInput.isMonthError = true;
			}
			let dayErr = !dateInput.checkDay(day, month, year);
			if(dayErr){
				dateInput.isDayError = true;
			}
		}

		ok = !dateInput.isError && !dateInput.isMonthError && !dateInput.isDayError
		let toShowErr = dateInput.isError || dateInput.isMonthError || dateInput.isDayError


		if(toShowErr){
			let errorString_ = dateInput.isError ? dateInput.errorString :
													   dateInput.isMonthError ? dateInput.errorStringMonth :
																	 dateInput.isDayError ? dateInput.errorStringDay: ""

			tooltip.text = errorString_;

			let point = dateInput.mapToItem(null, 0, -8 - tooltip.componentHeight)
			tooltip.openTooltipWithCoord(point.x,point.y)
			closeTooltipPause.restart();
		}

		return ok;
	}

	function checkMonth(month){
		return month.match(dateInput.dateRegMonth) !== null;
	}

	function checkDay(day, month, year){
		let ok  = false;
		let regOk = day.match(dateInput.dateRegDay) !== null;
		if(!regOk){
			return false;
		}

		if(day[0] == "0"){
			day = day[1];
		}
		if(month[0] == "0"){
			month = month[1];
		}

		let dayNumber = Number(day);

		if(dayNumber > 31){
			return false;
		}
		if(dayNumber <= 28){
			return true;
		}

		if(Number(month) == 2){
			let isLeapYear_ = dateInput.isLeapYear(year);
			ok  = isLeapYear_ ? dayNumber <= 29 :  dayNumber <=28;
		}
		else if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12){
			ok = dayNumber <= 31
		}
		else {
			ok = dayNumber <= 30
		}

		return ok;
	}

	function isLeapYear(year){
		let isLeapYear;

		if(!(year % 400)){
			isLeapYear = true;
		}
		else if(!(year % 100)){
			isLeapYear = false;
		}
		else if(!(year % 4)){
			isLeapYear = true;
		}
		else{
			isLeapYear = false;
		}
		return isLeapYear;
	}

	Column{
		id: column;

		spacing: Style.size_smallMargin;

		Text{
			id: titleText;

			font.family: Style.fontFamily;
			font.pixelSize: Style.fontSizeNormal;
			color: Style.textColor;

			visible: dateInput.hasTitle;

			text: qsTr("Date:");
		}

		Item{
			width: dateInput.width;
			height: dateInput.inputHeight;

			CustomTextField{
				id: input

				width: dateInput.width - calendarButtonContainer.width;
				height: dateInput.inputHeight;

				textSize: dateInput.fontSize;
				fontColor: dateInput.fontColor;
				//borderColorConst:  (dateInput.isError || dateInput.isMonthError || dateInput.isDayError) ? Style.errorTextColor : input.textInputActiveFocus ? Style.iconColorOnSelected: Style.borderColor;
				borderColor: Style.iconColorOnSelected;

				KeyNavigation.tab: dateInput.tabKeyItem;

				placeHolderText: dateInput.placeHolderText;
				placeHolderTextSize: fontSize-2;

				textInputValidator : RegularExpressionValidator { regularExpression: dateInput.dateRegExp }

				property bool isEmpty: false;

				onVisibleChanged: {
					if(text == ""){
						isEmpty = true;
					}
				}

				onFocusChanged: {
					if(!focus){
						dateInput.setDateAsString(input.text);
					}
				}
				onAccepted: {
					if(dateInput.setDateAsString(input.text)){
						dateInput.tabKeyItem.forceActiveFocus();
					}
				}

				onTextChanged: {
					if(text !== ""){
						isEmpty = false;
						dateInput.isError = false;
						dateInput.isMonthError = false;
						dateInput.isDayError = false;
					}

				}

				Rectangle{
					id: frame;

					anchors.fill: parent;

					radius: input.radius;
					color: "transparent";
					border.color:(dateInput.isError || dateInput.isMonthError || dateInput.isDayError) ? Style.errorTextColor : "transparent";
				}
			}//TextField

			Item {
				id: calendarButtonContainer;

				anchors.verticalCenter: parent.verticalCenter;
				anchors.left: input.right;
				width: 30;
				height: parent.height;
				clip: true;
				Rectangle{
					anchors.left: parent.left;
					anchors.leftMargin: -20;

					width: 50;
					height: parent.height;
					radius: input.radius;
					border.color: Style.borderColor;
				}
				Button{
					id: calendarButton;

					anchors.centerIn: parent;

					width: parent.width - 8;
					height: width;

					iconSource: "../../../" + Style.getIconPath("Icons/Calendar", Icon.State.Off, Icon.Mode.Normal);
					decorator: Component{IconButtonDecorator{}}
					onClicked: {
						let point = input.mapToItem(null, 0, input.height + 2);
						let parameters = {"x": point.x, "y":point.y};

						ModalDialogManager.openDialog(calendarComp, parameters);

					}

				}
			}
		}

	}

	CustomTooltip{
		id: tooltip;

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
		id: calendarComp;

		Calendar{
			width: 350;
			height: 350;
			hiddenBackground: true;
			centered: false;
			onAccepted: {
				dateInput.setDate(date);
			}
		}
	}
}
