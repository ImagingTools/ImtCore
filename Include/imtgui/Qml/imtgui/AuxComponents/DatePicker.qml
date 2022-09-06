import QtQuick 2.12
import Acf 1.0

Item {

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

    property int selectedIndexMonth: new Date().getMonth();

    property alias selectedYear: yearField.text;
    property alias selectedMonth: monthField.text;
    property alias selectedDay: dayField.text;

    property alias currentDayButtonVisible: currentDate.visible;

    property bool completed: false;

    signal dateChanged();

    Component.onCompleted: {
        let currentDate = new Date();
        yearField.text = currentDate.getFullYear();
        selectedIndexMonth = currentDate.getMonth();
        dayField.text = currentDate.getDate();

        completed = true;
    }

    onSelectedYearChanged: {
        console.log("onSelectedYearChanged");
        if (completed){
            dateChanged();
        }
    }

    onSelectedDayChanged: {
        if (completed){
            dateChanged();
        }
    }

    onSelectedIndexMonthChanged: {
        if (completed){
            dateChanged();
        }
    }

    function getDate(){
        let monthStr = String(selectedIndexMonth + 1)
        if (monthStr.length == 1){
            monthStr = "0" + monthStr;
        }

        let dayStr = String(selectedDay)
        if (dayStr.length == 1){
            dayStr = "0" + dayStr;
        }

        return selectedYear + "-" + monthStr + "-" + dayStr;
    }

    function checkDate(year, month, day){
        let date = new Date(year, month, day);

        return date.getFullYear() == year &&
               date.getMonth() == month &&
               date.getDate() == day;
    }

    function setDate(year, month, day){
        if (checkDate(year, month, day)){
            yearField.text = year;
            selectedIndexMonth = month;
            dayField.text = day;
        }
    }

    Row {
        height: 20;

        spacing: 10;

        RegExpValidator {
            id: yearValid;

            regExp: /^(19|20)\d{2}$/;
        }

        Row {
            height: parent.height;

            CustomTextField {
                id: yearField;

                height: 20;
                width: 42;

                textInputValidator: yearValid;

                Keys.onUpPressed: {
                    yearUpButton.clicked();
                }

                Keys.onDownPressed: {
                    yearDownButton.clicked();
                }
            }

            Column {
                AuxButton {
                    id: yearUpButton;

                    width: 15;
                    height: 10;

                    iconWidth: 12;
                    iconHeight: 10;

                    iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Up" + "_On_Normal.svg";

                    onClicked: {
                        let selectedYear = Number(yearField.text);

                        let newYear = String(selectedYear + 1);

                        if (checkDate(newYear, selectedIndexMonth, Number(dayField.text))){
                            yearField.text = newYear;
                        }
                    }
                }

                AuxButton {
                    id: yearDownButton;

                    width: 15;
                    height: 10;

                    iconWidth: 12;
                    iconHeight: 10;

                    iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg";

                    onClicked: {
                        if (yearField.text == ""){
                            return;
                        }

                        let selectedYear = Number(yearField.text);

                        let newYear = String(selectedYear - 1);

                        if (checkDate(newYear, selectedIndexMonth, Number(dayField.text))){
                            yearField.text = newYear;
                        }
                    }
                }
            }

            CustomTextField {
                id: monthField;

                height: 20;
                width: 85;

                readOnly: true;
                horizontalAlignment: TextInput.AlignHCenter;

                text: monthNames[selectedIndexMonth];

                Keys.onUpPressed: {
                    monthUpButton.clicked();
                }

                Keys.onDownPressed: {
                    monthDownButton.clicked();
                }
            }

            Column {
                AuxButton {
                    id: monthUpButton;

                    width: 15;
                    height: 10;

                    iconWidth: 12;
                    iconHeight: 10;

                    iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Up" + "_On_Normal.svg";

                    onClicked: {

                        let index;
                        if (selectedIndexMonth < 11){
                            index = selectedIndexMonth + 1;
                        }
                        else{
                            index = 0;
                        }

                        if (checkDate(Number(yearField.text), index, Number(dayField.text))){
                            selectedIndexMonth = index;
                        }
                    }
                }

                AuxButton {
                    id: monthDownButton;

                    width: 15;
                    height: 10;

                    iconWidth: 12;
                    iconHeight: 10;

                    iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg";

                    onClicked: {
                        let index;
                        if (selectedIndexMonth > 0){
                            index = selectedIndexMonth - 1;
                        }
                        else{
                            index = 11;
                        }

                        if (checkDate(Number(yearField.text), index, Number(dayField.text))){
                            selectedIndexMonth = index;
                        }
                    }
                }
            }

            RegExpValidator {
                id: dayValid;

                regExp: /\b([1-9]|[12][0-9]|3[01])\b/;
            }

            CustomTextField {
                id: dayField;

                height: 20;
                width: 25;

                horizontalAlignment: TextInput.AlignHCenter;
                textInputValidator: dayValid;

                Keys.onUpPressed: {
                    dayUpButton.clicked();
                }

                Keys.onDownPressed: {
                    dayDownButton.clicked();
                }
            }

            Column {
                id: dayButtons;

                AuxButton {
                    id: dayUpButton;

                    width: 15;
                    height: 10;

                    iconWidth: 12;
                    iconHeight: 10;

                    iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Up" + "_On_Normal.svg";

                    onClicked: {
                        let selectedDay = Number(dayField.text);
                        if (selectedDay < 31){
                            let newDay = String(selectedDay + 1)

                            if (checkDate(Number(yearField.text), selectedIndexMonth, Number(newDay))){
                                 dayField.text = newDay;
                            }
                        }
                    }
                }

                AuxButton {
                    id: dayDownButton;

                    width: 15;
                    height: 10;

                    iconWidth: 12;
                    iconHeight: 10;

                    iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg";

                    onClicked: {
                        let selectedDay = Number(dayField.text);
                        if (selectedDay > 1){
                            let newDay = String(selectedDay - 1);

                            if (checkDate(Number(yearField.text), selectedIndexMonth, Number(newDay))){
                                 dayField.text = newDay;
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            id: currentDate;

            anchors.verticalCenter: parent.verticalCenter;

            width: 7;
            height: width;

            radius: width;

            color: Style.textColor;

            MouseArea {
                anchors.fill: parent;

                cursorShape: Qt.PointingHandCursor;

                onClicked: {
                    let date = new Date();
                    setDate(date.getFullYear(), date.getMonth(), date.getDate());
                }
            }
        }
    }
}
