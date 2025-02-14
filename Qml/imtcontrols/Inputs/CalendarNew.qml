import QtQuick 2.12
import QtGraphicalEffects 1.12
import Qt5Compat.GraphicalEffects

import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0


Rectangle {
	id: calendar;

	width: 400;
	height: 480;
	radius: 8;

	border.width: 1;
	border.color: "#f6f6f6";

	property bool centered: true;
	property Item root: null;

	property bool multiSelection: false;
	property bool canFlick: false;
	property bool hasTopMenu: true;
	property bool hasButtonMenu: true;
	property bool gridOnly: false;
	property bool canShowToday: true;
	property bool readOnly: false;

	property int startYear: 1900;
	property int lastYear: 2100;

	//read only
	property int selectedIndexMonth: 0;
	property int selectedIndexYear: 0;
	property string selectedMonthName: "";

	property int selectedDay: 0;
	property int selectedMonth: 0;
	property int selectedYear: 0;

	property int endDay: 0;
	property int endMonth: 0;
	property int endYear: 0;

	property var today;
	property int todayDay: 0;
	property int todayMonth: 0;
	property int todayYear: 0;

	property bool selectedDateExist: false;
	property bool endDateExist: false;
	property bool startNotFinishDate: true;

	property string maxMonthName:"";
	property int listViewContentX: 0;

	property var monthNames: monthNamesEng;

	property var monthNamesEng:
		[
		qsTr("January"),
		qsTr("February"),
		qsTr("March"),
		qsTr("April"),
		qsTr("May"),
		qsTr("June"),
		qsTr("July"),
		qsTr("August"),
		qsTr("September"),
		qsTr("October"),
		qsTr("November"),
		qsTr("December")
	];

	property var monthNamesRus:
		[
		qsTr("Январь"),
		qsTr("Февраль"),
		qsTr("Март"),
		qsTr("Апрель"),
		qsTr("Май"),
		qsTr("Июнь"),
		qsTr("Июль"),
		qsTr("Август"),
		qsTr("Сентябрь"),
		qsTr("Октябрь"),
		qsTr("Ноябрь"),
		qsTr("Декабрь")
	];

	property var dayOfWeek: dayOfWeekEng;

	property var dayOfWeekRus:
		[
		qsTr("Пн"),
		qsTr("Вт"),
		qsTr("Ср"),
		qsTr("Чт"),
		qsTr("Пт"),
		qsTr("Сб"),
		qsTr("Вс"),
	];
	property var dayOfWeekEng:
		[
		qsTr("Mo"),
		qsTr("Tu"),
		qsTr("We"),
		qsTr("Th"),
		qsTr("Fr"),
		qsTr("Sa"),
		qsTr("Su"),
	];
	//read only


	//decoration
	property string mainColor: "#4682B4";
	property string cellColor: "#ffffff";
	property string selectColor: "#4682B4";
	property string topPanelColor: "#4682B4";
	property string buttonColor: "#4682B4";
	property string highlightColor: "#B0C4DE";
	property string fontColor_title: "#ffffff";
	property string fontColor_cell: "#000000";
	property string fontColor_button: "#ffffff";
	property int fontSize_cell: 12;
	property int fontSize_title: 24;
	property int fontSize_button: 14;
	property bool fontBold_title: false;
	property int buttonWidth: 100;
	property int buttonHeight: 35;
	property int buttonRadius: 6;
	property int weekDayListHeight: 50;
	property alias leftImageSource: leftButton.iconSource;
	property alias rightImageSource: rightButton.iconSource;
	property int bodyWidth: mainRec.width;
	property string okButtonText: qsTr("Ok");
	property string cancelButtonText: qsTr("Cancel");
	property string todayButtonText: qsTr("Today");

	//comboBox
	property bool hasMonthCombo: true;
	property bool hasYearCombo: true;
	property int shownItemsCountCombo: 12;
	property alias monthCombo: monthComboObj;
	property alias yearCombo: yearComboObj;

	property alias topPanel: topPanelObj;
	property alias topPanelHeight: topPanelObj.height;

	property bool compl: false;


	//OUTPUT
	property var dateStart;
	property var dateFinish;

	property var date: dateStart;

	property string dateStartStr: "";
	property string dateFinishStr: "";
	property string dateStr: dateStartStr;
	//OUTPUT

	property var calendarModel: [];

	property Component weekDayCellDelegate : weekDayCellComp;
	property Component dayCellDelegate: dayCellComp;

	signal accepted();
	signal canceled();
	//


	Component.onCompleted: {

		calendar.setMaxMonthName();

		let date = new Date();

		calendar.dateStart = date;
		calendar.dateFinish = date;
		calendar.today = date;

		calendar.todayDay = date.getDate();
		calendar.todayMonth = date.getMonth();
		calendar.todayYear = date.getFullYear();

		calendar.selectedIndexYear = date.getFullYear();
		calendar.selectedIndexMonth = date.getMonth();
		calendar.selectedMonthName  = calendar.monthName(calendar.selectedIndexMonth);
		topPanelTextMonth.text = calendar.monthName(calendar.selectedIndexMonth);

		if(calendar.startYear < 1 || calendar.lastYear < 1){
			calendar.startYear = 1900;
			calendar.lastYear = 2100;
		}
		if(calendar.startYear > calendar.todayYear || calendar.lastYear < calendar.todayYear){
			calendar.startYear = 1900;
			calendar.lastYear = 2100;
		}

		if(calendar.startYear > calendar.lastYear){
			let temp = calendar.startYear;
			calendar.startYear = calendar.lastYear;
			calendar.lastYear = temp;
		}

		calendar.fillMonthModel(calendar.selectedIndexMonth, calendar.selectedIndexYear);
		listview.canFillModel = true;

		calendar.fillYearComboModel();
		calendar.fillMonthComboModel();

		for(let i = 0; i < dayOfWeek.length; i++){
			let name = dayOfWeek[i];
			dayOfWeekListModel.append({"name": name});
		}

	}

	onRootChanged: {
		calendar.root.backgroundItem.opacity = 0.4;
	}

	onVisibleChanged: {
		if(calendar.visible && !calendar.root){
			let date = new Date();

			calendar.dateStart = date;
			calendar.dateFinish = date;
			calendar.today = date;

			calendar.todayDay = date.getDate();
			calendar.todayMonth = date.getMonth();
			calendar.todayYear = date.getFullYear();

			calendar.selectedIndexYear = date.getFullYear();
			calendar.selectedIndexMonth = date.getMonth();
			calendar.selectedMonthName  = calendar.monthName(calendar.selectedIndexMonth);
			topPanelTextMonth.text = calendar.monthName(calendar.selectedIndexMonth);

		}
	}

	onSelectedIndexMonthChanged: {
		if(topPanelTextMonth.text !== calendar.monthName(calendar.selectedIndexMonth)){
			topPanelTextMonth.text = calendar.monthName(calendar.selectedIndexMonth);
		}
	}

	onSelectedIndexYearChanged: {
		if(topPanelTextYear.text !== calendar.selectedIndexYear){
			topPanelTextYear.text = calendar.selectedIndexYear;
		}
	}

	function setDate(date_){
		let year = date_.getFullYear();
		let month = date_.getMonth();
		let day = date_.getDate()

		goToMonth(month + 1, year);
		calendar.selectedDay = day
		calendar.selectedMonth = month;
		calendar.selectedYear = year;

		calendar.selectedDateExist = true;
		calendar.endDateExist = false;

		calendar.acceptFunction();
	}

	function goToMonth(month, year){
		//Numbers the months of the year starting with 1
		if(month < 1){
			month = 1
		}
		if(month > 12){
			month = 12
		}

		calendar.selectedIndexMonth = month - 1;
		calendar.selectedIndexYear = year;
		calendar.fillMonthModel(calendar.selectedIndexMonth,calendar.selectedIndexYear);
	}

	function acceptFunction(){
		let dateStart;
		let dateFinish;
		if((!calendar.selectedDateExist) && (!calendar.endDateExist))
		{
			dateStart = new Date();
			dateFinish = new Date();
		}
		else if (calendar.selectedDateExist && (!calendar.endDateExist))
		{
			dateStart = new Date(calendar.selectedYear, calendar.selectedMonth, calendar.selectedDay);
			dateFinish = new Date(calendar.selectedYear, calendar.selectedMonth, calendar.selectedDay);

		}
		else if((!calendar.selectedDateExist) && calendar.endDateExist)
		{
			dateStart = new Date(calendar.endYear, calendar.endMonth, calendar.endDay);
			dateFinish = new Date(calendar.endYear, calendar.endMonth, calendar.endDay);
		}
		else
		{
			dateStart = new Date(calendar.selectedYear, calendar.selectedMonth, calendar.selectedDay);
			dateFinish = new Date(calendar.endYear, calendar.endMonth, calendar.endDay);
		}

		if(dateStart > dateFinish)
		{
			let dateTemp;

			dateTemp = dateStart;
			dateStart = dateFinish;
			dateFinish = dateTemp;

		}

		//ВЫХОДНЫЕ ДАННЫЕ
		calendar.dateStart = dateStart;
		calendar.dateFinish = dateFinish;
		calendar.dateStartStr = calendar.getDateStr(calendar.dateStart);
		calendar.dateFinishStr = calendar.getDateStr(calendar.dateFinish);

		//console.log("__________CALENDAR:______");
		//console.log(calendar.format(calendar.dateStart.getDate()) + "." + calendar.format(calendar.dateStart.getMonth() + 1) + "." + calendar.dateStart.getFullYear());
		//console.log(calendar.format(calendar.dateFinish.getDate()) + "." + calendar.format(calendar.dateFinish.getMonth() + 1) + "." + calendar.dateFinish.getFullYear());
		//console.log(calendar.getDateStr(calendar.dateStart));
		//console.log(calendar.getDateStr(calendar.dateFinish));
	}

	function close(){
		if(calendar.root){
			calendar.root.closeDialog();
		}
		else{
			calendar.visible = false;
			calendar.reset();
		}
	}

	function reset()
	{
		calendar.selectedDateExist = false;
		calendar.endDateExist = false;
		calendar.startNotFinishDate = true;

		calendar.selectedDay = 0;
		calendar.selectedMonth = 0;
		calendar.selectedYear = 0;

		calendar.endDay = 0;
		calendar.endMonth = 0;
		calendar.endYear = 0;

		let date = new Date();

		calendar.dateStart = date;
		calendar.dateFinish = date;
		calendar.today = date;
		calendar.todayDay = date.getDate();
		calendar.todayMonth = date.getMonth();
		calendar.todayYear = date.getFullYear();

		calendar.selectedIndexYear = date.getFullYear();
		calendar.selectedIndexMonth = date.getMonth();
		calendar.selectedMonthName  = calendar.monthName(calendar.selectedIndexMonth);

		calendar.fillMonthModel(calendar.selectedIndexMonth,calendar.selectedIndexYear);
		listview.canFillModel = true;

	}

	function setMaxMonthName(){
		let maxVal = "";
		for(let i = 0; i < calendar.monthNames.length; i++){
			let currVal = calendar.monthNames[i];
			if(currVal.length > maxVal.length){
				maxVal = currVal;
			}
		}
		calendar.maxMonthName = maxVal;
	}



	function fillMonthComboModel(){
		if(calendar.hasMonthCombo){
			for(let i = 0; i < calendar.monthNames.length; i++){
				let index = monthComboModel.insertNewItem();
				monthComboModel.setData("Id", index, index);
				monthComboModel.setData("Name",qsTr(calendar.monthNames[i]),index);
			}
		}
	}


	function fillYearComboModel(){
		if(calendar.hasYearCombo){
			for(let i = calendar.startYear; i <= calendar.lastYear;i++){
				let index = yearComboModel.insertNewItem();
				yearComboModel.setData("Id", index, index);
				yearComboModel.setData("Name", String(i),index);
			}
		}
	}

	function getDateStr(date){
		let dateStr;
		let day = calendar.format(date.getDate());
		let month = calendar.format(date.getMonth() + 1);
		let year = String(date.getFullYear());
		dateStr = year + "-" + month + "-" + day;
		return dateStr;
	}

	function lessThanDate (y1, m1 ,d1, y2, m2, d2){


		return (y1 < y2) ||
				(y1 <= y2 && (m1 < m2)) ||
				(y1 <= y2 && (m1 <= m2) && (d1 < d2));
	}

	function moreThanDate (y1, m1 ,d1, y2, m2, d2){


		return (y1 > y2) ||
				(y1 >= y2 && (m1 > m2)) ||
				(y1 >= y2 && (m1 >= m2) && (d1 > d2));

	}

	function monthName(ind){
		let month;
		month = monthNames[ind]
		return month;
	}

	function weekDayNumber(number){
		if(number == 0){
			return 7;
		}
		else{
			return number;
		}
	}

	function format(num){
		let retval = Number(num) < 10 ? "0" + String(num) :String(num);
		return retval;
	}


	function fillMonthModel(month, year){

		//console.log("_________FILL_MONTH_MODEL____________");

		listview.canFillModel = false;

		let prevMonth = month == 0 ? 11 : month - 1;
		let prevMonth_year = month == 0 ? year -1 : year;

		let daysPrev = calendar.fillDayModel(prevMonth, prevMonth_year);
		let daysCurrent = calendar.fillDayModel(month, year);

		let nextMonth = month == 11 ? 0 : month + 1;
		let nextMonth_year = month == 11 ? year + 1 : year;

		let daysNext = calendar.fillDayModel(nextMonth, nextMonth_year);

		let prevMonthObj = {
			DayModel: daysPrev,
			MonthName: calendar.monthName(prevMonth),
			Month: prevMonth,
			Year: prevMonth_year,
		}
		let currentMonthObj = {
			DayModel: daysCurrent,
			MonthName: calendar.monthName(month),
			Month: month,
			Year: year,
		}
		let nextMonthObj = {
			DayModel: daysNext,
			MonthName: calendar.monthName(nextMonth),
			Month: nextMonth,
			Year: nextMonth_year,
		}

		let monthModel = [];

		if(calendar.canFlick){
			monthModel = [
						prevMonthObj,
						currentMonthObj,
						nextMonthObj,
					]

		}
		else {
			monthModel = [
						currentMonthObj
					]
		}

		listview.model = monthModel;

		calendar.calendarModel = monthModel;

		if(calendar.canFlick){
			listview.positionViewAtIndex(1,ListView.Beginning);//!!!
		}

		calendar.listViewContentX = listview.contentX;

		calendar.selectedMonthName = calendar.monthName(calendar.selectedIndexMonth);

		listview.canFillModel = true;

	}



	function fillDayModel(month, year){
		let daysModel = [];

		let date  = new Date(year, month, 1);
		let firstWeekDay = calendar.weekDayNumber(date.getDay());

		let prevMonth = month == 0 ? 11 : month - 1;
		let prevMonth_year = month == 0 ? year -1 : year;
		let prevMonthLastDay = calendar.getLastDayOfMonth(prevMonth, prevMonth_year);

		let index = -1;

		for(let i = prevMonthLastDay - (firstWeekDay - 1 -1) ; i <= prevMonthLastDay; i++){
			index++;
			let day = {};
			day.Id = index;
			day.Day = i;
			day.Month = prevMonth;
			day.Year = prevMonth_year;
			day.CurrMonth = false;

			daysModel.push(day);

		}

		let lastDay = calendar.getLastDayOfMonth(month, year);
		for(let i = 1; i <= lastDay; i++){
			index++;
			let day = {};
			day.Id = index;
			day.Day = i;
			day.Month = month;
			day.Year = year;
			day.CurrMonth = true;

			daysModel.push(day);

		}

		let nextMonth = month == 11 ? 0 : month + 1;
		let nextMonth_year = month == 11 ? year + 1 : year;

		let count = daysModel.length;

		for(let i = 1; i <= (42 - count); i++){
			index++;
			let day = {};
			day.Id = index;
			day.Day = i;
			day.Month = nextMonth;
			day.Year = nextMonth_year;
			day.CurrMonth = false;

			daysModel.push(day);

		}
		return daysModel;
	}

	function getLastDayOfMonth(month,year){

		let lastDay;

		switch (month)
		{
		case 0 :lastDay = 31
			break
		case 1 :lastDay = 28
			break
		case 2 :lastDay = 31
			break
		case 3 :lastDay = 30
			break
		case 4 :lastDay = 31
			break
		case 5 :lastDay = 30
			break
		case 6 :lastDay = 31
			break
		case 7 : lastDay = 31
			break
		case 8 : lastDay = 30
			break
		case 9 :lastDay = 31
			break
		case 10 :lastDay = 30
			break
		case 11 :lastDay = 31
			break
		}

		if(month === 1){
			let isLeapYear = calendar.isLeapYearFun(Number(year));

			if(isLeapYear){
				lastDay = 29;
			}

		}

		return (lastDay);

	}//

	function isLeapYearFun(year){
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

	TreeItemModel {
		id: monthComboModel;
	}

	TreeItemModel {
		id: yearComboModel;
	}

	MouseArea{
		anchors.fill: parent;
		hoverEnabled: true;
		onClicked: {
			mouse.accepted = true;
		}
		onDoubleClicked: {
			mouse.accepted = true;
		}
		onWheel: {
			wheel.accepted = true;
		}
	}

	DropShadow {
		id: dropShadow;

		anchors.fill: forShadowRec;

		visible: forShadowRec.visible;
		horizontalOffset: 2;
		verticalOffset: 2;

		radius: forShadowRec.radius;
		color: "gray";

		source: forShadowRec;
	}

	Rectangle{
		id: forShadowRec;

		anchors.fill: parent;
		radius: parent.radius;
		color: parent.color;
		visible: !calendar.gridOnly;
	}

	ListModel{
		id: dayOfWeekListModel;
	}

	Rectangle{
		id: mainRec;

		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.top: parent.top;
		anchors.bottom: parent.bottom;

		anchors.leftMargin: mainMargin;
		anchors.rightMargin: mainMargin;
		anchors.topMargin: mainMargin;
		anchors.bottomMargin: mainMargin;

		color: calendar.color;

		property int mainMargin: calendar.gridOnly ? 0 : 10;

		Rectangle{
			id: topPanelObj;

			width: parent.width;
			height: visible ? parent.height/7 : 1;

			color: calendar.topPanelColor;
			radius: calendar.radius;
			visible: !calendar.gridOnly;
			clip: true;

			Item{
				id: topPanelTextItem;

				anchors.centerIn: parent;
				width: topPanelTextMonthItem.width + topPanelTextYearItem.width + topPanelTextYearItem.anchors.leftMargin;
				height: Math.max(topPanelTextMonthItem.height, topPanelTextYearItem.height);

				Item{
					id: topPanelTextMonthItem;

					anchors.verticalCenter: parent.verticalCenter;
					anchors.left: parent.left;

					height: topPanelTextMonth.height;
					width: topPanelTextMonth.width;

					Text {
						id: topPanelTextMonth;

						anchors.centerIn: parent;

						font.family: Style.fontFamily;
						font.pixelSize: calendar.fontSize_title;
						font.bold: calendar.fontBold_title;
						color: calendar.fontColor_title;
						//text: calendar.monthName(calendar.selectedIndexMonth);

					}

					Text {
						id: forSizeTextMonth;

						anchors.centerIn: parent;

						visible: false;

						font.family: Style.fontFamily;
						font.pixelSize: topPanelTextMonth.font.pixelSize;
						font.bold: topPanelTextMonth.font.bold;
						text: calendar.maxMonthName;


					}

					ComboBox {
						id: monthComboObj;

						anchors.centerIn:  parent;

						width: forSizeTextMonth.width;
						height: parent.height;

						visible: calendar.hasMonthCombo && !calendar.readOnly;
						enabled: visible;
						model: monthComboModel;

						textSize: calendar.fontSize_title -4;
						fontColor: calendar.fontColor_title;
						compMainColor: calendar.topPanelColor;
						compSelectedColor: calendar.highlightColor;

						borderColor: "transparent";
						backgroundColor: "transparent";
						visibleScrollBar: false;
						visibleIcon: false;
						isColor: true;
						contentLeftMargin: 0;

						fontColorTitle: "transparent";
						//compTextCentered: true;
						shownItemsCount: 12;
						//imageVisible: false;

						onCurrentIndexChanged:{
							if(monthComboObj.currentIndex >= 0){
								calendar.selectedIndexMonth = monthComboObj.currentIndex;
								calendar.fillMonthModel(calendar.selectedIndexMonth, calendar.selectedIndexYear);

							}
						}
					}//monthComboObj
				}

				Item{
					id: topPanelTextYearItem;

					anchors.verticalCenter: parent.verticalCenter;
					anchors.left: topPanelTextMonthItem.right;
					anchors.leftMargin: 20;

					height: topPanelTextYear.height;
					width: topPanelTextYear.width// + 30;

					Text {
						id: topPanelTextYear;

						anchors.centerIn: parent;

						font.family: Style.fontFamily;
						font.pixelSize: calendar.fontSize_title;
						font.bold: calendar.fontBold_title;
						color: calendar.fontColor_title;
						//text: calendar.selectedIndexYear;

					}


					ComboBox {
						id: yearComboObj;

						anchors.centerIn:  parent;

						width: parent.width;
						height: parent.height;

						visible: calendar.hasYearCombo && !calendar.readOnly;
						enabled: visible;
						model: yearComboModel;

						textSize: calendar.fontSize_title -4;
						fontColor: calendar.fontColor_title;
						compMainColor: calendar.topPanelColor;
						compSelectedColor: calendar.highlightColor;

						borderColor: "transparent";
						backgroundColor: "transparent";
						visibleScrollBar: false;
						visibleIcon: false;
						isColor:true
						contentLeftMargin: 0;

						fontColorTitle: "transparent";
						//compTextCentered: true;
						shownItemsCount: calendar.shownItemsCountCombo;
						//imageVisible: false;

						moveToIndex : calendar.todayYear - calendar.startYear - calendar.shownItemsCountCombo +1;

						onCurrentIndexChanged:{
							if(yearComboObj.currentIndex >=0){
								calendar.selectedIndexYear = yearComboObj.currentText;
								calendar.fillMonthModel(calendar.selectedIndexMonth, calendar.selectedIndexYear);

							}

						}
					}//yearComboObj
				}

			}

			Button {
				id: leftButton;

				anchors.verticalCenter: parent.verticalCenter;
				anchors.left: parent.left;
				anchors.leftMargin: 10;

				width: 25;
				height: width;

				enabled: !calendar.readOnly;

				decorator: Component{IconButtonDecorator{
						icon.width: !baseElement ? 0 : baseElement.width - 3;
						icon.height: !baseElement ? 0 : baseElement.height - 3;
					}}

				iconSource: "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Active);

				onClicked: {
					listview.decrease();
					calendar.fillMonthModel(calendar.selectedIndexMonth,calendar.selectedIndexYear)
				}
			}

			Button {
				id: rightButton;

				anchors.verticalCenter: parent.verticalCenter;
				anchors.right: parent.right;
				anchors.rightMargin: 10;

				width: 25;
				height: width;

				enabled: !calendar.readOnly;

				decorator: Component{IconButtonDecorator{
						icon.width: !baseElement ? 0 : baseElement.width - 3;
						icon.height: !baseElement ? 0 : baseElement.height - 3;
					}}

				iconSource: "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Active);

				onClicked: {
					listview.increase();
					calendar.fillMonthModel(calendar.selectedIndexMonth,calendar.selectedIndexYear)

				}
			}
		}


		ListView{
			id: listview;

			anchors.top: topPanelObj.bottom;
			anchors.left: parent.left;
			anchors.right: parent.right;
			anchors.bottom: buttonsContainer.top;

			clip: true;

			snapMode: ListView.SnapOneItem;
			orientation: ListView.Horizontal;
			boundsBehavior: Flickable.StopAtBounds;
			flickableDirection: Flickable.HorizontalFlick;
			property bool canFillModel: false;
			property bool canSetIndexes: true;
			property real movementStartX: 0;

			function setSelectedMothAndYear(){

				if(listview.canFillModel){
					if(contentX < calendar.listViewContentX){
						listview.decrease();
					}
					else if(contentX > calendar.listViewContentX){
						listview.increase();
					}
				}

			}

			function decrease(){

				let prevMonth = calendar.selectedIndexMonth == 0 ? 11 : calendar.selectedIndexMonth - 1;
				let prevMonth_year = calendar.selectedIndexMonth == 0 ? calendar.selectedIndexYear -1 : calendar.selectedIndexYear;

				if(calendar.selectedIndexYear !== 1){
					calendar.selectedIndexMonth = prevMonth;
					calendar.selectedIndexYear = prevMonth_year;
				}
				else if(calendar.selectedIndexMonth > 1){
					calendar.selectedIndexMonth = prevMonth;
					calendar.selectedIndexYear = prevMonth_year;
				}
				else {
					calendar.selectedIndexMonth = 0;
					calendar.selectedIndexYear = 1;
				}

			}

			function increase(){
				let nextMonth = calendar.selectedIndexMonth == 11 ? 0 : calendar.selectedIndexMonth + 1;
				let nextMonth_year = calendar.selectedIndexMonth == 11 ? calendar.selectedIndexYear + 1 : calendar.selectedIndexYear;
				calendar.selectedIndexMonth = nextMonth;
				calendar.selectedIndexYear = nextMonth_year;
			}


			onContentXChanged: {
				//console.log("___________CONTENT_X_____________", contentX);
				if(listview.canFillModel){
					if(contentX == calendar.listViewContentX - listview.width){


						if(listview.canSetIndexes){
							listview.setSelectedMothAndYear();
						}

						if(calendar.selectedIndexYear !== 1){
							calendar.fillMonthModel(calendar.selectedIndexMonth, calendar.selectedIndexYear);
						}
						else if(calendar.selectedIndexMonth > 0){
							calendar.fillMonthModel(calendar.selectedIndexMonth, calendar.selectedIndexYear);
						}
						else {
							calendar.selectedIndexMonth = 0;
							calendar.selectedIndexYear = 1;
						}
						listview.canSetIndexes = true;
					}
					else if(contentX == calendar.listViewContentX + listview.width){

						if(listview.canSetIndexes){
							listview.setSelectedMothAndYear();
						}

						calendar.fillMonthModel(calendar.selectedIndexMonth, calendar.selectedIndexYear);

						listview.canSetIndexes = true;
					}


				}

			}

			onMovementStarted: {
				listview.movementStartX = listview.contentX;
			}

			onFlickStarted: {
				let newMonth;
				if(listview.contentX > listview.movementStartX){
					newMonth = calendar.selectedIndexMonth + 1;
				}
				else {
					newMonth = calendar.selectedIndexMonth - 1;
				}
				let date = new Date(calendar.selectedIndexYear, newMonth);
				let monthIndex = date.getMonth();
				topPanelTextMonth.text = calendar.monthName(monthIndex);
				topPanelTextYear.text = date.getFullYear();
			}

			delegate: Rectangle{
				id: monthRec;

				width: listview.width;
				height: listview.height;
				radius: calendar.radius;

				ListView{
					id: dayOfWeekList;

					anchors.top: parent.top;
					anchors.left: parent.left;
					anchors.right: parent.right;

					height: calendar.weekDayListHeight;
					orientation: ListView.Horizontal;
					boundsBehavior: Flickable.StopAtBounds;
					clip: true;
					model: dayOfWeekListModel;

					delegate: calendar.weekDayCellDelegate;
				}

				GridView{
					id: grid;

					anchors.top: dayOfWeekList.bottom;
					anchors.bottom: parent.bottom;

					width: parent.width;
					cellWidth: Math.trunc(grid.width/7);
					cellHeight: Math.trunc(grid.height/6);
					boundsBehavior: Flickable.StopAtBounds;

					model: modelData.DayModel;
					delegate: calendar.dayCellDelegate;
				}//grid
			}
		}

		Item{
			id: buttonsContainer

			anchors.bottom: parent.bottom;

			width: parent.width;
			height: visible ? okButton.height + Style.size_smallMargin : 1;

			visible: !calendar.gridOnly;
			clip: true;

			Button {
				id: okButton;

				anchors.right: parent.right;
				anchors.bottom: parent.bottom;

				text: calendar.okButtonText;

				enabled: !calendar.readOnly

				onClicked:{
					calendar.acceptFunction();

					calendar.accepted();

					calendar.close();
				}
			}

			Button {
				id: cancelButton;

				anchors.right: okButton.left;
				anchors.bottom: parent.bottom;
				anchors.rightMargin: 10;

				enabled: !calendar.readOnly

				text: calendar.cancelButtonText;

				onClicked:{
					calendar.canceled();
					calendar.close();
				}
			}

			Button {
				id: todayButton;

				anchors.left: parent.left;
				anchors.bottom: parent.bottom;

				text: calendar.todayButtonText;

				enabled: !calendar.readOnly;

				onClicked:{
					let date = new Date();

					// выбор текущей даты
					calendar.selectedDay = date.getDate();
					calendar.selectedDateExist = true;
					calendar.endDateExist = false;

					let year = date.getFullYear();
					let month = date.getMonth();
					if(calendar.selectedYear !== year ||
							calendar.selectedIndexYear !== year ||
							calendar.selectedMonth !== month ||
							calendar.selectedIndexMonth !== month){

						calendar.selectedYear = year;
						calendar.selectedMonth = month;
						calendar.selectedIndexYear = year;
						calendar.selectedIndexMonth = month;
						calendar.fillMonthModel(month, year);

					}

				}
			}

		}//buttonsContainer

	}

	MouseArea{
		id: blockingMA;

		anchors.fill: parent;
		visible: false;
		enabled: visible;
		hoverEnabled: true;
		onClicked: {
			mouse.accepted = true;
		}
		onPressed: {
			mouse.accepted = true;
		}
		onPositionChanged: {
			mouse.accepted = true;

		}
		onPressAndHold: {
			mouse.accepted = true;

		}

		onWheel: {
			wheel.accepted = true;
		}

	}



	Shortcut {
		sequence: "Escape";
		enabled: true;
		onActivated: {
			calendar.canceled();
			calendar.close();
		}
	}

	Component{
		id: weekDayCellComp;

		Rectangle{

			width: calendar.bodyWidth/7;
			height: calendar.weekDayListHeight;

			color: calendar.cellColor;

			Text{
				anchors.centerIn: parent;

				font.family: Style.fontFamily;
				font.pixelSize: calendar.fontSize_cell;
				font.bold: true;
				color: calendar.fontColor_cell;

				text: model.name;
			}
		}
	}

	Component{
		id: dayCellComp;

		CalendarCellTextDelegate{
			calendarItem: calendar;
		}

	}

}

