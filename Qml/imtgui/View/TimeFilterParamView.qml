import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12

PopupView {
    id: root;

    width: 260;
    height: content.height + 2 * Style.size_mainMargin;

    model: TreeItemModel {}

    signal accepted(string id, string name);
    signal cancelled();

    TreeItemModel {
        id: treeModel;
        Component.onCompleted: {
            root.updateViewModel();
        }
    }

    onModelChanged: {
        updateGui();
    }

    function updateGui(){
        if (root.model.containsKey("TimeRange")){
            fromDateItem.visible = true;
            let timeRangeModel = root.model.getData("TimeRange");

            if (timeRangeModel.containsKey("Begin")){
                let beginDate = timeRangeModel.getData("Begin")

                let dateStr = beginDate.split(" ")[0];
                let data = dateStr.split(".");

                fromDatePicker.setDate(Number(data[2]), Number(data[1]) - 1, Number(data[0]));
            }

            if (timeRangeModel.containsKey("End")){
                let endDate = timeRangeModel.getData("End")
                let dateStr = endDate.split(" ")[0];
                let data = dateStr.split(".");

                toDatePicker.setDate(Number(data[2]), Number(data[1]) - 1, Number(data[0]));
            }
        }

        listView.currentIndex = -1;
        if (root.model.containsKey("TimeUnit")){
            let unit = root.model.getData("TimeUnit")
            let mode = root.model.getData("InterpretationMode")

            for (let i = 0; i < treeModel.getItemsCount(); i++){
                let id = treeModel.getData("Id", i);
                let interpretationMode = treeModel.getData("InterpretationMode", i);
                if (unit == id && interpretationMode == mode){
                    listView.currentIndex = i;
                    break;
                }
            }
        }
    }

    function updateViewModel(){
        treeModel.clear();

        let index = treeModel.insertNewItem();
        treeModel.setData("Id", "Hour", index);
        treeModel.setData("Name", qsTr("Last hour"), index);
        treeModel.setData("InterpretationMode", "For", index);

        index = treeModel.insertNewItem();
        treeModel.setData("Id", "Day", index);
        treeModel.setData("Name", qsTr("Today"), index);
        treeModel.setData("InterpretationMode", "Current", index);

        index = treeModel.insertNewItem();
        treeModel.setData("Id", "Week", index);
        treeModel.setData("Name", qsTr("This week"), index);
        treeModel.setData("InterpretationMode", "Current", index);

        index = treeModel.insertNewItem();
        treeModel.setData("Id", "Month", index);
        treeModel.setData("Name", qsTr("This month"), index);
        treeModel.setData("InterpretationMode", "Current", index);

        index = treeModel.insertNewItem();
        treeModel.setData("Id", "Month", index);
        treeModel.setData("Name", qsTr("Last month"), index);
        treeModel.setData("InterpretationMode", "Last", index);

        index = treeModel.insertNewItem();
        treeModel.setData("Id", "Year", index);
        treeModel.setData("Name", qsTr("This year"), index);
        treeModel.setData("InterpretationMode", "Current", index);

        index = treeModel.insertNewItem();
        treeModel.setData("Id", "Year", index);
        treeModel.setData("Name", qsTr("Last year"), index);
        treeModel.setData("InterpretationMode", "Last", index);

        listView.model = treeModel;
    }

    Rectangle {
        id: background;
        anchors.fill: parent;
        color: Style.baseColor;
        radius: Style.buttonRadius;
        border.width: 1;
        border.color: Style.borderColor;
    }

    MouseArea {
        anchors.fill: background;
    }

    Item {
        id: body;
        anchors.fill: parent;
        anchors.margins: Style.size_mainMargin;

        Column {
            id: content;
            anchors.verticalCenter: parent.verticalCenter;
            width: parent.width;
            spacing: Style.size_mainMargin;

            ListView {
                id: listView;
                width: parent.width;
                height: 30 * count;
                boundsBehavior: Flickable.StopAtBounds;
                interactive: false;
                delegate: Component {
                    PopupMenuDelegate {
                        text: model.Name;
                        highlighted: listView.currentIndex === model.index;
                        selected: mouseArea.containsMouse;

                        onClicked: {
                            listView.currentIndex = model.index;
                            root.model.clear();
                            root.model.setData("TimeUnit", model.Id);
                            root.model.setData("InterpretationMode", model.InterpretationMode);
                            root.accepted(model.Id, model.Name);
                        }
                    }
                }
            }

            Rectangle {
                id: separator;
                width: parent.width;
                height: 1;
                color: Style.borderColor;
            }

            BaseText {
                text: qsTr("From");
                width: parent.width;
            }

            Item {
                id: selectDateItem;
                width: parent.width;
                height: 30;
                visible: !fromDateItem.visible;

                Button {
                    width: parent.width;
                    height: parent.height;
                    text: qsTr("Select a date");
                    onClicked: {
                        fromDateItem.visible = true;
                        toDatePicker.setCurrentDay();
                    }

                    decorator: Component {
                        ButtonDecorator {
                            color: Style.backgroundColor2;
                        }
                    }
                }
            }

            Item {
                id: fromDateItem;
                width: parent.width;
                height: 30;
                visible: false;

                DatePicker {
                    id: fromDatePicker;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    width: contentWidth;
                    height: parent.height;
                    hasDayCombo: false;
                    hasMonthCombo: false;
                    hasYearCombo: false;
                    mainMargin: Style.size_mainMargin;

                    function checkDate(year, month, day){
                        let date = new Date(year, month, day);

                        let ok1 = date.getFullYear() == year &&
                                  date.getMonth() == month &&
                                  date.getDate() == day;
                        let ok2 = false;
                        if (ok1){
                            let toDate = toDatePicker.getDate();
                            if (date.getTime() <= toDate.getTime()){
                                ok2 = true;
                            }
                        }

                        return ok1 && ok2;
                    }
                }
            }

            BaseText {
                text: qsTr("To");
                width: parent.width;
            }

            Item {
                width: parent.width;
                height: 30;

                DatePicker {
                    id: toDatePicker;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    width: contentWidth;
                    height: parent.height;
                    hasDayCombo: false;
                    hasMonthCombo: false;
                    hasYearCombo: false;
                    startWithCurrentDay: true;
                    mainMargin: Style.size_mainMargin;

                    function checkDate(year, month, day){
                        let date = new Date(year, month, day);

                        let ok1 = date.getFullYear() == year &&
                                  date.getMonth() == month &&
                                  date.getDate() == day;
                        let ok2 = false;
                        if (ok1){
                            let fromDate = fromDatePicker.getDate();
                            if (date.getTime() >= fromDate.getTime()){
                                ok2 = true;
                            }
                        }

                        return ok1 && ok2;
                    }
                }
            }

            Item {
                width: parent.width;
                height: 30;

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter;
                    height: 30;
                    spacing: Style.size_mainMargin;

                    Button {
                        id: applyButton;
                        text: qsTr("Apply");
                        enabled: fromDateItem.visible;
                        onClicked: {
                            root.model.clear();
                            let timeRangeModel = root.model.addTreeModel("TimeRange");
                            timeRangeModel.setData("Begin", root.formatDate(fromDatePicker.getDate()));
                            timeRangeModel.setData("End", root.formatDate(toDatePicker.getDate()));

                            root.accepted("TimeRange", fromDatePicker.getDateAsString() + " -> " + toDatePicker.getDateAsString());
                        }

                        decorator: Component {
                            ButtonDecorator {
                                color: Style.backgroundColor2;
                            }
                        }
                    }

                    Button {
                        id: clearButton;
                        text: qsTr("Clear");
                        enabled: fromDateItem.visible;
                        onClicked: {
                            fromDateItem.visible = false;
                            root.model.clear();
                            fromDatePicker.setCurrentDay();
                            toDatePicker.setCurrentDay();

                            root.cancelled();
                        }

                        decorator: Component {
                            ButtonDecorator {
                                color: Style.backgroundColor2;
                            }
                        }
                    }
                }
            }
        }
    }

    DropShadow {
        id: dropShadow;

        anchors.fill: background;

        z: background.z-1

        horizontalOffset: 2;
        verticalOffset: 2;

        radius: 4;
        color: Style.shadowColor;

        source: background;
    }

    function fromFormat(date){
        let dateStr = date.split(" ")[0];
        let data = dateStr.split(".");

        return data[2] + "-" + data[1] + "-" + data[0];
    }

    function formatDate(date){
        if (!date){
            return "";
        }

        let year = date.getFullYear()
        let month = date.getMonth();
        let day = date.getDate();

        let monthStr = String(month + 1)
        if (monthStr.length == 1){
            monthStr = "0" + monthStr;
        }

        let dayStr = String(day)
        if (dayStr.length == 1){
            dayStr = "0" + dayStr;
        }

        return dayStr + "." + monthStr + "." + year + " " + "00:00:00";
    }
}


