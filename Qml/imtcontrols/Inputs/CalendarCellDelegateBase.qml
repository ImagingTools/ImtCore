import QtQuick 2.12

import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle{
	id: dayDelegate;

	width: grid ? grid.cellWidth : 0;
	height: grid ? grid.cellHeight: 0;

	color: calendarItem.cellColor;

	property Item calendarItem: null;
	property Item grid: !parent ? null : parent.parent;

	property bool isSelectedDate:
		(modelData.Day === dayDelegate.calendarItem.selectedDay &&
		 modelData.Month === dayDelegate.calendarItem.selectedMonth &&
		 modelData.Year === dayDelegate.calendarItem.selectedYear) ? true : false;
	property bool isEndDate:

		(modelData.Day === dayDelegate.calendarItem.endDay &&
		 modelData.Month === dayDelegate.calendarItem.endMonth &&
		 modelData.Year === dayDelegate.calendarItem.endYear) ? true : false;


	property bool isToday: modelData.Day === dayDelegate.calendarItem.todayDay && modelData.Month === dayDelegate.calendarItem.todayMonth && modelData.Year === dayDelegate.calendarItem.todayYear;

	property real modelDay: modelData.Day;
	property real modelMonth: modelData.Month;
	property real modelYear: modelData.Year;

	property bool isMidlleDateLR: (dayDelegate.calendarItem.selectedDateExist
								   && dayDelegate.calendarItem.endDateExist
								   && dayDelegate.calendarItem.moreThanDate(modelData.Year, modelData.Month, modelData.Day, dayDelegate.calendarItem.selectedYear,dayDelegate.calendarItem.selectedMonth,dayDelegate.calendarItem.selectedDay)
								   && dayDelegate.calendarItem.lessThanDate(modelData.Year, modelData.Month, modelData.Day, dayDelegate.calendarItem.endYear,dayDelegate.calendarItem.endMonth,dayDelegate.calendarItem.endDay)
								   ) ? true : false;




	Rectangle {
		id: circle;

		anchors.fill: parent;

		color: (dayDelegate.isSelectedDate || (dayDelegate.isEndDate && dayDelegate.calendarItem.endDateExist)) ?
				   dayDelegate.calendarItem.selectColor : " transparent";
		radius:  dayDelegate.calendarItem.height/7;
		border.color: dayDelegate.calendarItem.mainColor;
		border.width: (dayDelegate.isToday && dayDelegate.calendarItem.canShowToday) ? 2 :0;


		Rectangle {
			id: rectangleLeft;

			anchors.verticalCenter: parent.verticalCenter;
			anchors.left: parent.left;

			height: parent.height;
			width: parent.width/2;
			color: (dayDelegate.isMidlleDateLR || (dayDelegate.isEndDate && dayDelegate.calendarItem.endDateExist))?
					   dayDelegate.calendarItem.selectColor : "transparent";


		}

		Rectangle {
			id: rectangleRight;

			anchors.right: parent.right;
			anchors.verticalCenter: parent.verticalCenter;

			height: parent.height;
			width: parent.width/2;
			color: (dayDelegate.isMidlleDateLR || (dayDelegate.isSelectedDate && dayDelegate.calendarItem.endDateExist))?
					   dayDelegate.calendarItem.selectColor : "transparent";

		}

	}

	MouseArea {
		anchors.fill:parent;

		visible: !dayDelegate.calendarItem.readOnly

		onClicked:{

			if(!dayDelegate.calendarItem.multiSelection)
			{
				dayDelegate.calendarItem.selectedDateExist = true;

				dayDelegate.calendarItem.selectedDay = dayDelegate.modelDay;
				dayDelegate.calendarItem.selectedMonth = dayDelegate.modelMonth;
				dayDelegate.calendarItem.selectedYear = dayDelegate.modelYear;
			}
			else
			{
				if (!dayDelegate.calendarItem.selectedDateExist){

					dayDelegate.calendarItem.selectedDateExist = true;

					dayDelegate.calendarItem.selectedDay = dayDelegate.modelDay;
					dayDelegate.calendarItem.selectedMonth = dayDelegate.modelMonth;
					dayDelegate.calendarItem.selectedYear = dayDelegate.modelYear;


				} else if(!dayDelegate.calendarItem.endDateExist)
				{

					if (dayDelegate.calendarItem.moreThanDate(dayDelegate.modelYear, dayDelegate.modelMonth, dayDelegate.modelDay,
											  dayDelegate.calendarItem.selectedYear, dayDelegate.calendarItem.selectedMonth, dayDelegate.calendarItem.selectedDay))

					{
						dayDelegate.calendarItem.endDateExist = true;

						dayDelegate.calendarItem.endDay = dayDelegate.modelDay;
						dayDelegate.calendarItem.endMonth = dayDelegate.modelMonth;
						dayDelegate.calendarItem.endYear = dayDelegate.modelYear;

					} else if (dayDelegate.calendarItem.lessThanDate(dayDelegate.modelYear, dayDelegate.modelMonth, dayDelegate.modelDay,
													 dayDelegate.calendarItem.selectedYear, dayDelegate.calendarItem.selectedMonth, dayDelegate.calendarItem.selectedDay))

					{
						dayDelegate.calendarItem.endDateExist = true;

						let year;
						let month;
						let day;

						year = dayDelegate.calendarItem.selectedYear;
						month =  dayDelegate.calendarItem.selectedMonth;
						day = dayDelegate.calendarItem.selectedDay;

						dayDelegate.calendarItem.selectedYear = dayDelegate.modelYear;
						dayDelegate.calendarItem.selectedMonth = dayDelegate.modelMonth;
						dayDelegate.calendarItem.selectedDay = dayDelegate.modelDay;

						dayDelegate.calendarItem.endYear = year;
						dayDelegate.calendarItem.endMonth = month;
						dayDelegate.calendarItem.endDay = day;


					} else {

						dayDelegate.calendarItem.selectedDateExist = true;

						dayDelegate.calendarItem.selectedDay = dayDelegate.modelDay;
						dayDelegate.calendarItem.selectedMonth = dayDelegate.modelMonth;
						dayDelegate.calendarItem.selectedYear = dayDelegate.modelYear;

					}

				} else

				{
					dayDelegate.calendarItem.selectedDateExist = true;
					dayDelegate.calendarItem.endDateExist = false;

					dayDelegate.calendarItem.selectedDay = dayDelegate.modelDay;
					dayDelegate.calendarItem.selectedMonth = dayDelegate.modelMonth;
					dayDelegate.calendarItem.selectedYear = dayDelegate.modelYear;

				}

			}
		}

	}
}
