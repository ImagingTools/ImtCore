import QtQuick 2.12
import QtGraphicalEffects 1.12

import Acf 1.0
import imtgui 1.0
import imtqml 1.0


Rectangle {
    id: calendar;

    width:400;
    height: 480;
    radius: 8;

    border.width: 1;
    border.color: "#f6f6f6";

    property bool centered: true;
    property Item root: null;

    property bool simpleCalendar: true;
    property bool interactive: true;

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

    property var monthNames: monthNamesRus;

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

    property var dayOfWeek: dayOfWeekRus;

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
    property bool fontBold_title: true;
    property int buttonWidth: 100;
    property int buttonHeight: 35;
    property int buttonRadius: 6;
    property alias leftImageSource: leftButton.iconSource;
    property alias rightImageSource: rightButton.iconSource;
    property string okButtonText: "Ок";
    property string cancelButtonText: "Отмена";
    property string todayButtonText: "Сегодня";

    //comboBox
    property bool hasMonthCombo: true;
    property bool hasYearCombo: true;
    property int shownItemsCountCombo: 12;
    property alias monthCombo: monthComboObj;
    property alias yearCombo: yearComboObj;

    property alias topPanel: topPanelObj;

    property bool compl: false;


    //OUTPUT
    property var dateStart;
    property var dateFinish;

    property var date: dateStart;

    property string dateStartStr: "";
    property string dateFinishStr: "";
    property string dateStr: dateStartStr;
    //OUTPUT

    property TreeItemModel monthTreeModel: TreeItemModel{};
    property TreeItemModel monthTreeModel_temp1: TreeItemModel{};
    property TreeItemModel monthTreeModel_temp2: TreeItemModel{};

    signal accepted();
    signal canceled();
    //


    Component.onCompleted: {

            calendar.setMaxMonthName();

            var date = new Date();

            calendar.dateStart = date;
            calendar.dateFinish = date;
            calendar.today = date;

            calendar.todayDay = date.getDate();
            calendar.todayMonth = date.getMonth();
            calendar.todayYear = date.getFullYear();

            calendar.selectedIndexYear = date.getFullYear();
            calendar.selectedIndexMonth = date.getMonth();
            calendar.selectedMonthName  = calendar.monthName(calendar.selectedIndexMonth);

            if(calendar.startYear < 1 || calendar.lastYear < 1){
                calendar.startYear = 1900;
                calendar.lastYear = 2100;
            }
            if(calendar.startYear > calendar.todayYear || calendar.lastYear < calendar.todayYear){
                calendar.startYear = 1900;
                calendar.lastYear = 2100;
            }

            if(calendar.startYear > calendar.lastYear){
                var temp = calendar.startYear;
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
        if(calendar.visible){
            var date = new Date();

            calendar.today = date;

            calendar.todayDay = date.getDate();
            calendar.todayMonth = date.getMonth();
            calendar.todayYear = date.getFullYear();
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

        var date = new Date();

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
        var maxVal = "";
        for(var i = 0; i < calendar.monthNames.length; i++){
            var currVal = calendar.monthNames[i];
            if(currVal.length > maxVal.length){
                maxVal = currVal;
            }
        }
        calendar.maxMonthName = maxVal;
    }

    function fillMonthComboModel(){
        if(calendar.hasMonthCombo){
            for(var i = 0; i < calendar.monthNames.length; i++){
                var index = monthComboModel.InsertNewItem();
                monthComboModel.SetData("Id", index, index);
                monthComboModel.SetData("Name",qsTr(calendar.monthNames[i]),index);
            }
        }
    }


    function fillYearComboModel(){
        if(calendar.hasYearCombo){
            for(var i = calendar.startYear; i <= calendar.lastYear;i++){
                let index = yearComboModel.InsertNewItem();
                yearComboModel.SetData("Id", index, index);
                yearComboModel.SetData("Name", String(i),index);
            }
        }
    }

    function getDateStr(date){
        var dateStr;
        var day = calendar.format(date.getDate());
        var month = calendar.format(date.getMonth() + 1);
        var year = String(date.getFullYear());
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
        var retval = Number(num) < 10 ? "0" + String(num) :String(num);
        return retval;
    }

    function fillMonthModel(month, year){

        //console.log("_________FILL_MONTH_MODEL____________");

        listview.canFillModel = false;

        calendar.monthTreeModel.Clear();

        var prevMonth = month == 0 ? 11 : month - 1;
        var prevMonth_year = month == 0 ? year -1 : year;

        calendar.fillDayModel(dayTreeModelPrev,prevMonth, prevMonth_year);
        calendar.fillDayModel(dayTreeModel, month, year);

        var nextMonth = month == 11 ? 0 : month + 1;
        var nextMonth_year = month == 11 ? year + 1 : year;

        calendar.fillDayModel(dayTreeModelNext, nextMonth, nextMonth_year);

        //calendar.monthTreeModel.Clear();

        var modelToFill;
        var isFirstTempModel = true;
        if(!calendar.monthTreeModel_temp1.GetItemsCount()){
            modelToFill = calendar.monthTreeModel_temp1;
            isFirstTempModel = true;
        }
        else{
            modelToFill = calendar.monthTreeModel_temp2;
            isFirstTempModel = false;
        }

        var index = modelToFill.InsertNewItem();
        modelToFill.SetData("DayModel", dayTreeModelPrev, index);
        modelToFill.SetData("MonthName", calendar.monthName(prevMonth), index);
        modelToFill.SetData("Month", prevMonth, index);
        modelToFill.SetData("Year", prevMonth_year , index);

        index = modelToFill.InsertNewItem();
        modelToFill.SetData("DayModel", dayTreeModel, index);
        modelToFill.SetData("MonthName", calendar.monthName(month), index);
        modelToFill.SetData("Month", month, index);
        modelToFill.SetData("Year", year, index);

        index = modelToFill.InsertNewItem();
        modelToFill.SetData("DayModel", dayTreeModelNext, index);
        modelToFill.SetData("MonthName", calendar.monthName(nextMonth), index);
        modelToFill.SetData("Month", nextMonth, index);
        modelToFill.SetData("Year", nextMonth_year , index);


        modelToFill.SetIsArray(true);

        calendar.monthTreeModel = modelToFill;
        if(isFirstTempModel){
            calendar.monthTreeModel_temp2.Clear();
        }
        else{
            calendar.monthTreeModel_temp1.Clear();
        }


        //console.log("____________MonthModel______________");
        //        console.log(calendar.monthTreeModel.toJSON());



        listview.positionViewAtIndex(1,ListView.Beginning);
        calendar.listViewContentX = listview.contentX;

        //calendar.selectedIndexMonth = calendar.monthTreeModel.GetData("Month",1);
        calendar.selectedMonthName = calendar.monthName(calendar.selectedIndexMonth);
        //calendar.selectedIndexYear = calendar.monthTreeModel.GetData("Year",1);

        listview.canFillModel = true;

    }



    function fillDayModel(model, month, year){
        model.Clear();

        var date  = new Date(year, month, 1);
        var firstWeekDay = calendar.weekDayNumber(date.getDay());

        var prevMonth = month == 0 ? 11 : month - 1;
        var prevMonth_year = month == 0 ? year -1 : year;
        var prevMonthLastDay = calendar.getLastDayOfMonth(prevMonth, prevMonth_year);


        for(var i = prevMonthLastDay - (firstWeekDay - 1 -1) ; i <= prevMonthLastDay; i++){
            var index = model.InsertNewItem();
            model.SetData("Id", index, index);
            model.SetData("Day",i,index);
            model.SetData("Month",prevMonth,index);
            model.SetData("Year",prevMonth_year,index);
            model.SetData("CurrMonth",false,index);

        }

        var lastDay = calendar.getLastDayOfMonth(month, year);
        for(i = 1; i <= lastDay; i++){
            index = model.InsertNewItem();
            model.SetData("Id", index, index);
            model.SetData("Day",i,index);
            model.SetData("Month",month,index);
            model.SetData("Year",year,index);
            model.SetData("CurrMonth",true,index);


        }

        var nextMonth = month == 11 ? 0 : month + 1;
        var nextMonth_year = month == 11 ? year + 1 : year;

        var count = model.GetItemsCount();

        for(i = 1; i <= (42 - count); i++){
            index = model.InsertNewItem();
            model.SetData("Id", index, index);
            model.SetData("Day",i,index);
            model.SetData("Month",nextMonth,index);
            model.SetData("Year",nextMonth_year,index);
            model.SetData("CurrMonth",false,index);

        }

    }

    function getLastDayOfMonth(month,year){

        var lastDay;

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
            var isLeapYear = calendar.isLeapYearFun(Number(year));

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
        id: monthComboModel;
    }

    TreeItemModel {
        id: yearComboModel;
    }


    TreeItemModel {
        id: dayTreeModel;
    }

    TreeItemModel {
        id: dayTreeModelPrev;
    }

    TreeItemModel {
        id: dayTreeModelNext;
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
        visible: true;
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

        anchors.leftMargin: 10;
        anchors.rightMargin: 10;
        anchors.topMargin: 10;
        anchors.bottomMargin: 10;

        Rectangle{
            id: topPanelObj;
            width: parent.width;
            height: parent.height/7;
            color: calendar.topPanelColor;
            radius: calendar.radius;
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

                        visible: calendar.hasMonthCombo;
                        enabled: visible;
                        model: monthComboModel;

                        textSize: calendar.fontSize_title -4;
                        fontColor: calendar.fontColor_title;
                        compMainColor: calendar.topPanelColor;
                        compSelectedColor: calendar.highlightColor;

                        borderColor: "transparent";
                        backgroundColor: "transparent";
                        visibleScrollBar: false;
                        isColor: true;

                        titleTxtColor: "transparent";
                        compTextCentered: true;
                        shownItemsCount: 12;
                        imageVisible: false;

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
                    width: topPanelTextYear.width;

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

                        visible: calendar.hasYearCombo;
                        enabled: visible;
                        model: yearComboModel;
                        anchors.fill: parent;

                        textSize: calendar.fontSize_title -4;
                        fontColor: calendar.fontColor_title;
                        compMainColor: calendar.topPanelColor;
                        compSelectedColor: calendar.highlightColor;

                        borderColor: "transparent";
                        backgroundColor: "transparent";
                        visibleScrollBar: false;
                        isColor:true

                        titleTxtColor: "transparent";
                        compTextCentered: true;
                        shownItemsCount: calendar.shownItemsCountCombo;
                        imageVisible: false;

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

            AuxButton {
                id: leftButton;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 10;

                highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;
                width: 25;
                height: width;

                iconSource: "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Active);

                onClicked: {
                    listview.decrease();
                    listview.canSetIndexes = false;
                    listview.contentX -= listview.width;
                }
            }

            AuxButton {
                id: rightButton;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;
                anchors.rightMargin: 10;

                highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;
                width: 25;
                height: width;

                iconSource: "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Active);

                onClicked: {
                    listview.increase();
                    listview.canSetIndexes = false;
                    listview.contentX += listview.width;
                }
            }


        }


        ListView{
            id: listview;

            anchors.top: topPanelObj.bottom;
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.bottom: okButton.top;
            anchors.bottomMargin: Style.size_mainMargin;

            clip: true;
            interactive: true;//calendar.interactive;

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

                var prevMonth = calendar.selectedIndexMonth == 0 ? 11 : calendar.selectedIndexMonth - 1;
                var prevMonth_year = calendar.selectedIndexMonth == 0 ? calendar.selectedIndexYear -1 : calendar.selectedIndexYear;

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
                var nextMonth = calendar.selectedIndexMonth == 11 ? 0 : calendar.selectedIndexMonth + 1;
                var nextMonth_year = calendar.selectedIndexMonth == 11 ? calendar.selectedIndexYear + 1 : calendar.selectedIndexYear;
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
                var date = new Date(calendar.selectedIndexYear, newMonth);
                let monthIndex = date.getMonth();
                topPanelTextMonth.text = calendar.monthName(monthIndex);
                topPanelTextYear.text = date.getFullYear();
            }

            model: calendar.monthTreeModel;
            delegate: Rectangle{
                id: monthRec;

                width: listview.width;
                height: listview.height;
                radius: calendar.radius;
                property TreeItemModel emptyModel: TreeItemModel{};
                property TreeItemModel dayModel:  model.DayModel !== undefined ? model.DayModel: emptyModel;

                ListView{
                    id: dayOfWeekList;

                    anchors.top: parent.top;
                    anchors.left: parent.left;
                    anchors.right: parent.right;

                    height: 50;
                    orientation: ListView.Horizontal;
                    boundsBehavior: Flickable.StopAtBounds;
                    clip: true;
                    model: dayOfWeekListModel;
                    delegate: Rectangle{

                        width: grid.cellWidth;
                        height: dayOfWeekList.height;
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

                GridView{
                    id: grid;

                    anchors.top: dayOfWeekList.bottom;
                    anchors.bottom: parent.bottom;

                    width: parent.width;
                    cellWidth: grid.width/7;
                    cellHeight: grid.height/6;
                    boundsBehavior: Flickable.StopAtBounds;

                    model: monthRec.dayModel;
                    delegate: Rectangle{
                        id: dayDelegate;

                        width: grid.cellWidth;
                        height: grid.cellHeight;
                        color: calendar.cellColor;

                        property bool isSelectedDate:
                            (model.Day === calendar.selectedDay &&
                             model.Month === calendar.selectedMonth &&
                             model.Year === calendar.selectedYear) ? true : false;
                        property bool isEndDate:

                            (model.Day === calendar.endDay &&
                             model.Month === calendar.endMonth &&
                             model.Year === calendar.endYear) ? true : false;


                        //property bool isToday: model.Day === calendar.today.getDate() && model.Month === calendar.today.getMonth() && model.Year === calendar.today.getFullYear();
                        property bool isToday: model.Day === calendar.todayDay && model.Month === calendar.todayMonth && model.Year === calendar.todayYear;


                        property real modelDay: model.Day;
                        property real modelMonth: model.Month;
                        property real modelYear: model.Year;

                        property bool isMidlleDateLR: (calendar.selectedDateExist
                                                       && calendar.endDateExist
                                                       && calendar.moreThanDate(model.Year, model.Month, model.Day, calendar.selectedYear,calendar.selectedMonth,calendar.selectedDay)
                                                       && calendar.lessThanDate(model.Year, model.Month, model.Day, calendar.endYear,calendar.endMonth,calendar.endDay)
                                                       ) ? true : false;




                        Rectangle {
                            id: circle;

                            anchors.fill: parent;

                            color: (dayDelegate.isSelectedDate || (dayDelegate.isEndDate && calendar.endDateExist)) ?
                                       calendar.selectColor : " transparent";
                            radius:  calendar.height/7;
                            border.color: calendar.mainColor;
                            border.width: dayDelegate.isToday ? 2 :0;


                            Rectangle {
                                id: rectangleLeft;

                                anchors.verticalCenter: parent.verticalCenter;
                                anchors.left: parent.left;

                                height: parent.height;
                                width: parent.width/2;
                                color: (dayDelegate.isMidlleDateLR || (dayDelegate.isEndDate && calendar.endDateExist))?
                                           calendar.selectColor : "transparent";


                            }

                            Rectangle {
                                id: rectangleRight;

                                anchors.right: parent.right;
                                anchors.verticalCenter: parent.verticalCenter;

                                height: parent.height;
                                width: parent.width/2;
                                color: (dayDelegate.isMidlleDateLR || (dayDelegate.isSelectedDate && calendar.endDateExist))?
                                           calendar.selectColor : "transparent";

                            }

                        }

                        Text{
                            anchors.centerIn: parent;

                            font.family: Style.fontFamily;
                            font.pixelSize: calendar.fontSize_cell;
                            color: calendar.fontColor_cell;
                            opacity: model.CurrMonth ? 1: 0.5;

                            text: model.Day;
                        }

                        MouseArea {
                            anchors.fill:parent;

                            onClicked:{

                                if(calendar.simpleCalendar)
                                {
                                    calendar.selectedDateExist = true;

                                    calendar.selectedDay = dayDelegate.modelDay;
                                    calendar.selectedMonth = dayDelegate.modelMonth;
                                    calendar.selectedYear = dayDelegate.modelYear;
                                }
                                else
                                {
                                    if (!calendar.selectedDateExist){

                                        calendar.selectedDateExist = true;

                                        calendar.selectedDay = dayDelegate.modelDay;
                                        calendar.selectedMonth = dayDelegate.modelMonth;
                                        calendar.selectedYear = dayDelegate.modelYear;


                                    } else if(!calendar.endDateExist)
                                    {

                                        if (calendar.moreThanDate(dayDelegate.modelYear, dayDelegate.modelMonth, dayDelegate.modelDay,
                                                                  calendar.selectedYear, calendar.selectedMonth, calendar.selectedDay))

                                        {
                                            calendar.endDateExist = true;

                                            calendar.endDay = dayDelegate.modelDay;
                                            calendar.endMonth = dayDelegate.modelMonth;
                                            calendar.endYear = dayDelegate.modelYear;

                                        } else if (calendar.lessThanDate(dayDelegate.modelYear, dayDelegate.modelMonth, dayDelegate.modelDay,
                                                                         calendar.selectedYear, calendar.selectedMonth, calendar.selectedDay))

                                        {
                                            calendar.endDateExist = true;

                                            var year;
                                            var month;
                                            var day;

                                            year = calendar.selectedYear;
                                            month =  calendar.selectedMonth;
                                            day = calendar.selectedDay;

                                            calendar.selectedYear = dayDelegate.modelYear;
                                            calendar.selectedMonth = dayDelegate.modelMonth;
                                            calendar.selectedDay = dayDelegate.modelDay;

                                            calendar.endYear = year;
                                            calendar.endMonth = month;
                                            calendar.endDay = day;


                                        } else {

                                            calendar.selectedDateExist = true;

                                            calendar.selectedDay = dayDelegate.modelDay;
                                            calendar.selectedMonth = dayDelegate.modelMonth;
                                            calendar.selectedYear = dayDelegate.modelYear;

                                        }

                                    } else

                                    {
                                        calendar.selectedDateExist = true;
                                        calendar.endDateExist = false;

                                        calendar.selectedDay = dayDelegate.modelDay;
                                        calendar.selectedMonth = dayDelegate.modelMonth;
                                        calendar.selectedYear = dayDelegate.modelYear;

                                    }

                                }
                            }

                        }
                    }

                }

            }

        }

        BaseButton {
            id: okButton;

            anchors.right: parent.right;
            anchors.bottom: parent.bottom;

            decorator: Style.calendarButtonDecorator !==undefined ? Style.calendarButtonDecorator : defaultButtonDecorator;

            text: calendar.okButtonText;

            onClicked:{
                var dateStart;
                var dateFinish;
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
                    var dateTemp;

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

                calendar.accepted();

                calendar.close();
            }
        }

        BaseButton {
            id: cancelButton;

            anchors.right: okButton.left;
            anchors.bottom: parent.bottom;
            anchors.rightMargin: 10;

            decorator: Style.calendarButtonDecorator !==undefined ? Style.calendarButtonDecorator : defaultButtonDecorator;

            text: calendar.cancelButtonText;

            onClicked:{
                calendar.canceled();
                calendar.close();
            }
        }

        BaseButton {
            id: todayButton;

            anchors.left: parent.left;
            anchors.bottom: parent.bottom;

            decorator: Style.calendarButtonDecorator !==undefined ? Style.calendarButtonDecorator : defaultButtonDecorator;

            text: calendar.todayButtonText;

            onClicked:{
                var date = new Date();

                // выбор текущей даты
                calendar.selectedDay = date.getDate();
                calendar.selectedDateExist = true;
                calendar.endDateExist = false;

                var year = date.getFullYear();
                var month = date.getMonth();
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

        Component{
            id: defaultButtonDecorator;

            BaseButtonDecorator {
                id: commonButtonDecorator;

                width: calendar.buttonWidth;
                height: calendar.buttonHeight;

                Rectangle{
                    id: mainRec;

                    anchors.fill: parent;

                    radius: calendar.buttonRadius;
                    color: calendar.buttonColor;
                    Text {
                        id: mainText;

                        anchors.centerIn: parent;

                        font.family: Style.fontFamily;
                        font.pixelSize: calendar.fontSize_button;
                        font.bold: false;

                        color: calendar.fontColor_button;

                        text: commonButtonDecorator.text;
                    }
                }
            }

        }


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

    Rectangle{
        anchors.fill: parent;
        color: "#000000";
        opacity: 0.3;
        visible: false;//blockingComboMA.visible;
    }

    MouseArea{
        id: blockingComboMA;

        anchors.fill: parent;
        visible: yearComboObj.openST || monthComboObj.openST;
        enabled: visible;
        hoverEnabled: true;
        onClicked: {
            calendar.root.closeDialog();
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


}

