import QtQuick 2.12



QtObject{
	id: internal;

	property var monthNames:
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

	function monthName(ind){
		let month;
		month = monthNames[ind]
		return month;
	}

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
			let isLeapYear = internal.isLeapYearFun(Number(year));

			if(isLeapYear){
				lastDay = 29;
			}

		}

		return (lastDay);

	}

	function weekDayNumber(number){
		if(number == 0){
			return 7;
		}
		else{
			return number;
		}
	}

	function fillDayModel(month, year){
		let daysModel = [];

		let date  = new Date(year, month, 1);
		let firstWeekDay = internal.weekDayNumber(date.getDay());

		let prevMonth = month == 0 ? 11 : month - 1;
		let prevMonth_year = month == 0 ? year -1 : year;
		let prevMonthLastDay = internal.getLastDayOfMonth(prevMonth, prevMonth_year);

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

		let lastDay = internal.getLastDayOfMonth(month, year);
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

	function format(num){
		let retval = Number(num) < 10 ? "0" + String(num) :String(num);
		return retval;
	}

	function getDateStr(date){
		let dateStr;
		let day = internal.format(date.getDate());
		let month = internal.format(date.getMonth() + 1);
		let year = String(date.getFullYear());
		dateStr = year + "-" + month + "-" + day;
		return dateStr;
	}

	function fillMonthData(month, year){
		let days = internal.fillDayModel(month, year);

		let monthObj = {
			DayModel: days,
			MonthName: internal.monthName(month),
			Month: month,
			Year: year,
		}

		return monthObj;
	}

}

