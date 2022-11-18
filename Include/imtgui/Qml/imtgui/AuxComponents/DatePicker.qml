import QtQuick 2.12
import Acf 1.0

Item {
    id: datePicker;

    height: 20;
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

    //icons
    property int iconWidth: 12;
    property int iconHeight: 10;
    property string iconUpSource: "../../../" + "Icons/" + Style.theme + "/" + "Up" + "_On_Normal.svg";
    property string iconDownSource: "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg";
    //margins
    property int mainMargin: 0;
    //CustomTextField
    property int textFieldHeight: 20;
    property int textFieldWidthYear: 42;
    property int textFieldWidthMonth: 85;
    property int textFieldWidthDay: 25;
    property int textFieldRadius:0;
    property int textSize: 15;
    property string textFieldColor: Style.baseColor;
    property string fontColor: Style.textColor;
    property string textFieldBorderColor: "";//textField.acceptableInput ? Style.iconColorOnSelected : Style.errorTextColor;
    property int textFieldMargin: 5;
    //buttons
    property int buttonWidth: 15;
    property int buttonHeight: 10;

    property string buttonColor:"";
    property int buttonHighlighted: -1;
    //

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
        height: datePicker.height;

        spacing: 10;

        RegExpValidator {
            id: yearValid;

            regExp: /^(19|20)\d{2}$/;
        }

        Row {
            id: mainRow;
            height: parent.height;
            spacing: datePicker.mainMargin;

            CustomTextField {
                id: yearField;

                anchors.verticalCenter: parent.verticalCenter;

                height: datePicker.textFieldHeight;
                width: datePicker.textFieldWidthYear;
                radius: datePicker.textFieldRadius;
                textSize: datePicker.textSize;
                fontColor: datePicker.fontColor;
                color: datePicker.textFieldColor;
                borderColor: datePicker.textFieldBorderColor !== "" ? datePicker.textFieldBorderColor : acceptableInput ? Style.iconColorOnSelected : Style.errorTextColor;
                margin: datePicker.textFieldMargin;

                textInputValidator: yearValid;

                Keys.onUpPressed: {
                    yearUpButton.clicked();
                }

                Keys.onDownPressed: {
                    yearDownButton.clicked();
                }
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter;
                AuxButton {
                    id: yearUpButton;

                    width: datePicker.buttonWidth;
                    height: datePicker.buttonHeight;

                    iconWidth: datePicker.iconWidth;
                    iconHeight: datePicker.iconHeight;
                    iconSource: datePicker.iconUpSource;

                    highlighted: datePicker.buttonHighlighted >= 0 ? datePicker.buttonHighlighted : containsMouse;
                    color: datePicker.buttonColor !== "" ? datePicker.buttonColor : defaultColor;

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

                    width: datePicker.buttonWidth;
                    height: datePicker.buttonHeight;

                    iconWidth: datePicker.iconWidth;
                    iconHeight: datePicker.iconHeight;
                    iconSource: datePicker.iconDownSource;

                    highlighted: datePicker.buttonHighlighted >= 0 ? datePicker.buttonHighlighted : containsMouse;
                    color: datePicker.buttonColor !== "" ? datePicker.buttonColor : defaultColor;


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

                anchors.verticalCenter: parent.verticalCenter;

                height: datePicker.textFieldHeight;
                width: datePicker.textFieldWidthMonth;
                radius: datePicker.textFieldRadius;
                textSize: datePicker.textSize;
                fontColor: datePicker.fontColor;
                color: datePicker.textFieldColor;
                borderColor: datePicker.textFieldBorderColor !== "" ? datePicker.textFieldBorderColor : acceptableInput ? Style.iconColorOnSelected : Style.errorTextColor;
                margin: datePicker.textFieldMargin;

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
                anchors.verticalCenter: parent.verticalCenter;
                AuxButton {
                    id: monthUpButton;

                    width: datePicker.buttonWidth;
                    height: datePicker.buttonHeight;

                    iconWidth: datePicker.iconWidth;
                    iconHeight: datePicker.iconHeight;
                    iconSource: datePicker.iconUpSource;

                    highlighted: datePicker.buttonHighlighted >= 0 ? datePicker.buttonHighlighted : containsMouse;
                    color: datePicker.buttonColor !== "" ? datePicker.buttonColor : defaultColor;


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

                    width: datePicker.buttonWidth;
                    height: datePicker.buttonHeight;

                    iconWidth: datePicker.iconWidth;
                    iconHeight: datePicker.iconHeight;
                    iconSource: datePicker.iconDownSource;

                    highlighted: datePicker.buttonHighlighted >= 0 ? datePicker.buttonHighlighted : containsMouse;
                    color: datePicker.buttonColor !== "" ? datePicker.buttonColor : defaultColor;


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

                anchors.verticalCenter: parent.verticalCenter;

                height: datePicker.textFieldHeight;
                width: datePicker.textFieldWidthDay;
                radius: datePicker.textFieldRadius;
                textSize: datePicker.textSize;
                fontColor: datePicker.fontColor;
                color: datePicker.textFieldColor;
                borderColor: datePicker.textFieldBorderColor !== "" ? datePicker.textFieldBorderColor : acceptableInput ? Style.iconColorOnSelected : Style.errorTextColor;
                margin: datePicker.textFieldMargin;

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

                anchors.verticalCenter: parent.verticalCenter;

                AuxButton {
                    id: dayUpButton;

                    width: datePicker.buttonWidth;
                    height: datePicker.buttonHeight;

                    iconWidth: datePicker.iconWidth;
                    iconHeight: datePicker.iconHeight;
                    iconSource: datePicker.iconUpSource;

                    highlighted: datePicker.buttonHighlighted >= 0 ? datePicker.buttonHighlighted : containsMouse;
                    color: datePicker.buttonColor !== "" ? datePicker.buttonColor : defaultColor;


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

                    width: datePicker.buttonWidth;
                    height: datePicker.buttonHeight;

                    iconWidth: datePicker.iconWidth;
                    iconHeight: datePicker.iconHeight;
                    iconSource: datePicker.iconDownSource;

                    highlighted: datePicker.buttonHighlighted >= 0 ? datePicker.buttonHighlighted : containsMouse;
                    color: datePicker.buttonColor !== "" ? datePicker.buttonColor : defaultColor;


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
