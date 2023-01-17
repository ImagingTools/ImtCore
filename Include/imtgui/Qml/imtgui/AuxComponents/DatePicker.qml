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

    property int startYear: 1900;
    property int lastYear: 2100;

    property int todayYear;

    property int selectedIndexMonth;

    property alias selectedYear: yearField.text;
    property alias selectedMonth: monthField.text;
    property alias selectedDay: dayField.text;

    property alias currentDayButtonVisible: currentDate.visible;
    property alias defaultWidth: mainRow.width;
    property alias moveToEndYear : yearCombo.moveToEnd;
    property alias moveToIndexYear : yearCombo.moveToIndex;

    property bool completed: false;

    property bool startWithCurrentDay: true;
    property bool startWithFirstDay: false;
    property bool hasDay: true;

    property bool hasDayCombo: false;
    property bool hasMonthCombo: false;
    property bool hasYearCombo: false;
    property int shownItemsCountCombo: 12;
    property alias monthCombo: monthCombo;
    property alias yearCombo: yearCombo;
    property alias dayCombo: dayCombo;

    property bool openST: monthCombo.openST || yearCombo.openST || dayCombo.openST;

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
        console.log("onCompleted");

        var date_ = new Date();

        datePicker.selectedIndexMonth = date_.getMonth();
        datePicker.todayYear = date_.getFullYear();

        if (datePicker.startWithCurrentDay){
            let currentDate = new Date();
            yearField.text = currentDate.getFullYear();
            selectedIndexMonth = currentDate.getMonth();
            dayField.text = currentDate.getDate();
        }
        if(datePicker.startWithFirstDay){
            let currentDate = new Date();
            yearField.text = currentDate.getFullYear();
            selectedIndexMonth = currentDate.getMonth();
            dayField.text = "1";

        }

        if(datePicker.hasMonthCombo){
            for(var i = 0; i < datePicker.monthNames.length; i++){
                var index = monthTreeModel.InsertNewItem();
                monthTreeModel.SetData("Id", index, index);
                monthTreeModel.SetData("Name",qsTr(datePicker.monthNames[i]),index);
            }
        }

        if(datePicker.hasYearCombo && !yearCombo.model.GetItemsCount()){
            if(datePicker.startYear < 1 || datePicker.lastYear < 1){
                datePicker.startYear = 1900;
                datePicker.lastYear = 2100;
            }
            if(datePicker.startYear > datePicker.todayYear || datePicker.lastYear < datePicker.todayYear){
                datePicker.startYear = 1900;
                datePicker.lastYear = 2100;
            }

            if(datePicker.startYear > datePicker.lastYear){
                var temp = datePicker.startYear;
                datePicker.startYear = datePicker.lastYear;
                datePicker.lastYear = temp;
            }
            for(let i = datePicker.startYear; i <= datePicker.lastYear;i++){
                let index = yearTreeModel.InsertNewItem();
                yearTreeModel.SetData("Id", index, index);
                yearTreeModel.SetData("Name", String(i),index);
            }

        }

        if(datePicker.hasDayCombo && datePicker.selectedYear !== ""){
            datePicker.fillDayModel();

        }

        datePicker.completed = true;
    }

    onSelectedYearChanged: {
        console.log("onSelectedYearChanged");
        if (datePicker.completed){
            datePicker.dateChanged();
        }
        if(datePicker.hasDayCombo && datePicker.completed){
            datePicker.fillDayModel();
        }
    }

    onSelectedDayChanged: {
        if (datePicker.completed){
            datePicker.dateChanged();
        }
    }

    onSelectedIndexMonthChanged: {
        if (datePicker.completed){
            datePicker.dateChanged();
        }
        if(datePicker.hasDayCombo && datePicker.completed){
            datePicker.fillDayModel();
        }
    }

    function getDate(){
        let monthStr = String(datePicker.selectedIndexMonth + 1)
        if (monthStr.length == 1){
            monthStr = "0" + monthStr;
        }

        let dayStr = String(datePicker.selectedDay)
        if (dayStr.length == 1){
            dayStr = "0" + dayStr;
        }

        return datePicker.selectedYear + "-" + monthStr + "-" + dayStr;
    }

    function setCurrentDay(){
        let date = new Date();
        datePicker.setDate(date.getFullYear(), date.getMonth(), date.getDate());
    }

    function checkDate(year, month, day){
        let date = new Date(year, month, day);

        return date.getFullYear() == year &&
                date.getMonth() == month &&
                date.getDate() == day;
    }

    function setDate(year, month, day){
        if (datePicker.checkDate(year, month, day)){
            yearField.text = year;
            datePicker.selectedIndexMonth = month;
            dayField.text = day;
        }
    }

    function fillDayModel(){
        dayTreeModel.Clear();
        var lastDay = datePicker.getLastDayOfMonth(datePicker.selectedIndexMonth + 1);
        for(var i = 1; i <= lastDay; i++){
            var index = dayTreeModel.InsertNewItem();
            dayTreeModel.SetData("Id", i, index);
            dayTreeModel.SetData("Name",i,index);

        }

    }



    function getLastDayOfMonth(month){

        var lastDay;

        switch (month)
        {
        case 1 :lastDay = 31
            break
        case 2 :lastDay = 28
            break
        case 3 :lastDay = 31
            break
        case 4 :lastDay = 30
            break
        case 5 :lastDay = 31
            break
        case 6 :lastDay = 30
            break
        case 7 :lastDay = 31
            break
        case 8 : lastDay = 31
            break
        case 9 : lastDay = 30
            break
        case 10 :lastDay = 31
            break
        case 11 :lastDay = 30
            break
        case 12 :lastDay = 31
            break
        }

        if(month === 2){
            var isLeapYear = datePicker.isLeapYearFun(Number(datePicker.selectedYear));

            if(isLeapYear){
                lastDay = 29;
            }

        }


        return (lastDay);

    }//

    function isLeapYearFun(year){
        var isLeapYear;

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
        id: yearTreeModel;
    }

    TreeItemModel {
        id: monthTreeModel;
    }

    TreeItemModel {
        id: dayTreeModel;
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

                horizontalAlignment: TextInput.AlignHCenter;

                textInputValidator: yearValid;

                Keys.onUpPressed: {
                    yearUpButton.clicked();
                }

                Keys.onDownPressed: {
                    yearDownButton.clicked();
                }

                ComboBox {
                    id: yearCombo;

                    visible: datePicker.hasYearCombo;
                    enabled: visible;
                    model: yearTreeModel;
                    anchors.fill: parent;

                    textSize: datePicker.textSize;
                    fontColor: yearField.fontColor;
                    borderColor: "transparent";
                    backgroundColor: "transparent";
                    hiddenBackground: false;

                    titleTxt.color: "transparent";
                    compTextCentered: true;
                    shownItemsCount: datePicker.shownItemsCountCombo;
                    moveToIndex : datePicker.todayYear - datePicker.startYear - datePicker.shownItemsCountCombo +1;


                    onCurrentIndexChanged:{
                        if(yearCombo.currentIndex >=0){
                            let selectedYear = yearCombo.currentText;

                            if (datePicker.checkDate(selectedYear, datePicker.selectedIndexMonth, Number(dayField.text))){
                                yearField.text = selectedYear;
                            }
                        }

                    }
                }//yearCombo
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

                        if (datePicker.checkDate(newYear, datePicker.selectedIndexMonth, Number(dayField.text))){
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

                        if (datePicker.checkDate(newYear, datePicker.selectedIndexMonth, Number(dayField.text))){
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

                text: datePicker.monthNames[datePicker.selectedIndexMonth];

                Keys.onUpPressed: {
                    monthUpButton.clicked();
                }

                Keys.onDownPressed: {
                    monthDownButton.clicked();
                }

                ComboBox {
                    id: monthCombo;

                    visible: datePicker.hasMonthCombo;
                    enabled: visible;
                    model: monthTreeModel;
                    anchors.fill: parent;

                    textSize: datePicker.textSize;//Style.fontSize_common;
                    fontColor: monthField.fontColor;
                    borderColor: "transparent";
                    backgroundColor: "transparent";
                    hiddenBackground: false;

                    titleTxt.color: "transparent";
                    compTextCentered: true;
                    shownItemsCount: 12;

                    onCurrentIndexChanged:{
                        if(monthCombo.currentIndex >= 0){
                            if (datePicker.checkDate(Number(yearField.text), monthCombo.currentIndex, Number(dayField.text))){
                                datePicker.selectedIndexMonth = monthCombo.currentIndex;
                            }
                        }
                    }
                }//monthCombo
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
                        if (datePicker.selectedIndexMonth < 11){
                            index = datePicker.selectedIndexMonth + 1;
                        }
                        else{
                            index = 0;
                        }

                        if (datePicker.checkDate(Number(yearField.text), index, Number(dayField.text))){
                            datePicker.selectedIndexMonth = index;
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
                        if (datePicker.selectedIndexMonth > 0){
                            index = datePicker.selectedIndexMonth - 1;
                        }
                        else{
                            index = 11;
                        }

                        if (datePicker.checkDate(Number(yearField.text), index, Number(dayField.text))){
                            datePicker.selectedIndexMonth = index;
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

                visible: datePicker.hasDay;

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

                ComboBox {
                    id: dayCombo;

                    visible: datePicker.hasDayCombo;
                    enabled: visible;
                    model: dayTreeModel;
                    anchors.fill: parent;

                    textSize: datePicker.textSize;
                    fontColor: dayField.fontColor;
                    borderColor: "transparent";
                    backgroundColor: "transparent";
                    hiddenBackground: false;

                    titleTxt.color: "transparent";
                    compTextCentered: true;
                    shownItemsCount: datePicker.shownItemsCountCombo;

                    onCurrentIndexChanged:{
                        if(dayCombo.currentIndex >=0){
                            let selectedDay = dayCombo.currentText;

                            if (datePicker.checkDate(datePicker.selectedYear, datePicker.selectedIndexMonth, Number(selectedDay))){
                                dayField.text = selectedDay;
                            }
                        }

                    }
                }//yearCombo
            }

            Column {
                id: dayButtons;

                anchors.verticalCenter: parent.verticalCenter;
                visible: datePicker.hasDay;

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

                            if (datePicker.checkDate(Number(yearField.text), datePicker.selectedIndexMonth, Number(newDay))){
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

                            if (datePicker.checkDate(Number(yearField.text), datePicker.selectedIndexMonth, Number(newDay))){
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
                    datePicker.setCurrentDay();
                }
            }
        }
    }
}
