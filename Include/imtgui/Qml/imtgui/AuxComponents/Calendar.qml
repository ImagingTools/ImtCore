import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Rectangle {
    id: calendarContainer;

    height: 20;
    width: row.width;

    color: Style.backgroundColor;

    property var selectedDate;

    property var monthsNames: ['Январь', 'Февраль', 'Март', 'Апрель', 'Май', 'Июнь', 'Июль', 'Август', 'Сентябрь', 'Октябрь', 'Ноябрь', 'Декабрь'];

    signal dateChanged();

    Component.onCompleted: {
        console.log("Calendar onCompleted");
        calendarContainer.selectedDate = new Date("2023-01-01");

        var currentDate = new Date();
        var curYear = currentDate.getFullYear();
        var i;

        for (i = curYear; i < curYear + 5; i++){
            modelYear.append({"text": i});
        }

        for (i = 0; i < 12; i++){
           // modelMonth.append({"text": Qt.locale().standaloneMonthName(i, Locale.LongFormat)})

            modelMonth.append({"text": calendarContainer.monthsNames[i]})
        }

        yearCB.currentText = calendarContainer.selectedDate.getFullYear();
        monthCB.currentText = calendarContainer.monthsNames[0];
        dayCB.currentText = 1;

        for (i = 1; i <= 31; i++){
            modelDay.append({"text": i})
        }
    }

    function daysInMonth(iMonth, iYear) {
       return 32 - new Date(iYear, iMonth, 32).getDate();
    }

    function formatDate() {

      var dd = calendarContainer.selectedDate.getDate();
      if (dd < 10) dd = '0' + dd;

      var mm = calendarContainer.selectedDate.getMonth() + 1;
      if (mm < 10) mm = '0' + mm;

      var yyyy = calendarContainer.selectedDate.getFullYear();
      //if (yy < 10) yy = '0' + yy;

      return dd + '.' + mm + '.' + yyyy;
    }

    function setDate(date){
        console.log("Calendar setDate", date);

        if ((new Date(date) === "Invalid Date") ||
                isNaN(new Date(date))){
            console.log("Calendar setDate Invalid Date!");
            return;
        }

        var y = date.getFullYear();
        var m = date.getMonth();
        var d = date.getDate();
        var i;
        console.log(y, m, d);

        yearCB.currentText = y;
        monthCB.currentText = calendarContainer.monthsNames[m];
        dayCB.currentText = d;

        for (i = 0; i < modelYear.count; i++){
            if (Number(modelYear.get(i).text) === y){
                yearCB.currentIndex = i;
                break;
            }
        }

        monthCB.currentIndex = m;
        dayCB.currentIndex = d - 1;
    }

    Row {
        id: row;
//        radius: 3;

        ComboBox {
            id: yearCB;

            height: calendarContainer.height;
            width: 60;

            backgroundColor: Style.baseColor;
            menuHeight: 100;
            radius: 0;

            model: ListModel {
                id: modelYear;
            }

            borderColor: Style.theme == "Dark" ? "#565757" : "#a4a4a6";

            onCurrentIndexChanged: {
                console.log("YearCB onCurrentIndexChanged");

                var year = Number(yearCB.currentText);
                var date = new Date(year, monthCB.currentIndex, 1);

                if (monthCB.currentIndex === 1){

                    var day = calendarContainer.selectedDate.getDate();

                    var curCountDay = calendarContainer.daysInMonth(calendarContainer.selectedDate.getMonth(),
                                                                 calendarContainer.selectedDate.getFullYear());

                    var countDay = calendarContainer.daysInMonth(date.getMonth(), date.getFullYear());

                    if (countDay === 28 && curCountDay === 29){
                        if (day > 28){
                            dayCB.currentIndex = 0;
                            dayCB.currentText = 1;
                            calendarContainer.selectedDate.setDate(1);
                        }
                        modelDay.remove(28);
                    }
                    else if (countDay === 29 && curCountDay === 28){
                        modelDay.append({"text": 29})
                    }
                }
                calendarContainer.selectedDate.setFullYear(year);

                calendarContainer.dateChanged();
            }
        }

        ComboBox {
            id: monthCB;

            height: calendarContainer.height;
            width: 80;

            backgroundColor: Style.baseColor;
            menuHeight: 100;
            radius: 0;

            model: ListModel {
                id: modelMonth;
            }

            borderColor: Style.theme == "Dark" ? "#565757" : "#a4a4a6";

            onCurrentIndexChanged: {
                console.log("monthCB onCurrentIndexChanged");
                var date = new Date(calendarContainer.selectedDate.getFullYear(), monthCB.currentIndex, 1);

                var curCountDay = calendarContainer.daysInMonth(calendarContainer.selectedDate.getMonth(),
                                                             calendarContainer.selectedDate.getFullYear());


                var newCountDay = calendarContainer.daysInMonth(date.getMonth(), date.getFullYear());

                var curDay = calendarContainer.selectedDate.getDate();
                var countDay = calendarContainer.daysInMonth(date.getMonth(), date.getFullYear());

                if (curDay > countDay){
                    dayCB.currentIndex = 0;
                    dayCB.currentText = 1;
                    calendarContainer.selectedDate.setDate(1);
                }

                calendarContainer.selectedDate.setMonth(monthCB.currentIndex);

                modelDay.clear();
                for (var i = 1; i <= newCountDay; i++){
                    modelDay.append({"text": i})
                }

                calendarContainer.dateChanged();
            }
        }

        ComboBox {
            id: dayCB;

            height: calendarContainer.height;
            width: 40;

            menuHeight: 100;
            radius: 0;

            backgroundColor: Style.baseColor;

            model: ListModel {
                id: modelDay;
            }

            borderColor: Style.theme == "Dark" ? "#565757" : "#a4a4a6";

            onCurrentIndexChanged: {
                console.log("dayCB onCurrentIndexChanged");
                calendarContainer.selectedDate.setDate(dayCB.currentIndex + 1);

                calendarContainer.dateChanged();
            }
        }
    }
}
