import QtQuick 2.12
import Acf 1.0

Item {
    id: root;

    height: 100;
    width: row.width;

    property int days: 0;
    property int hours: 0;
    property int minutes: 0;

    property bool allCompleted: daysModel.completed && hoursModel.completed && minuitesModel.completed;

    signal intervalChanged();

    onDaysChanged: {
        root.intervalChanged();
    }

    onHoursChanged: {
        root.intervalChanged();
    }

    onMinutesChanged: {
        root.intervalChanged();
    }

    function getInSeconds(){
        let result = 0;
        if (root.days > 0){
            result += 86400 * root.days;
        }

        if (root.hours > 0){
            result += 3600 * root.hours;
        }

        if (root.minutes > 0){
            result += 60 * root.minutes;
        }

        return result;
    }

    function setDays(day){
        days.setValue(day);
    }

    function setHours(hour){
        hours.setValue(hour);
    }

    function setMinutes(minute){
        minutes.setValue(minute);
    }

    ListModel {
        id: daysModel;

        property bool completed: false;

        Component.onCompleted: {
            daysModel.append({ value: -1, text: " " })
            for(var i = 0; i <= 100; i++){
                var norm = i.toString();
                daysModel.append({ value: i, text: norm })
            }
            daysModel.append({ value: -1, text: " " })

            days.model = daysModel;

            daysModel.completed = true;
        }
    }

    ListModel {
        id: hoursModel;
         property bool completed: false;
        Component.onCompleted: {
            hoursModel.append({ value: -1, text: " " })
            for(var i = 0; i <= 23; i++){
                var norm = i.toString();
                if( i < 10 ) norm = "0" + i
                hoursModel.append({ value: i, text: norm })
            }
            hoursModel.append({ value: -1, text: " " })

            hours.model = hoursModel;

            hoursModel.completed = true;
        }
    }

    ListModel {
        id: minuitesModel;
        property bool completed: false;
        Component.onCompleted: {
            minuitesModel.append({ value: -1, text: " " })
            for(var i = 0; i <= 59; i++){
                var norm = i.toString();
                if( i < 10 ) norm = "0" + i
                minuitesModel.append({ value: i, text: norm })
            }
            minuitesModel.append({ value: -1, text: " " })

            minutes.model = minuitesModel;

            minuitesModel.completed = true;
        }
    }

    Row {
        id: row;

        height: root.height;

        Item {
            width: days.width;
            height: parent.height;

            Text {
                id: daysTitle;

                text: qsTr("Days");

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Spinner {
                id: days;

                anchors.top: daysTitle.bottom;
                anchors.topMargin: 5;

                onCurrentValueChanged: {
                    root.days = days.currentValue;
                }
            }
        }

        Item {
            width: hours.width;
            height: parent.height;

            Text {
                id: hoursTitle;

                text: qsTr("Hours");

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Spinner {
                id: hours;

                anchors.top: hoursTitle.bottom;
                anchors.topMargin: 5;

                onCurrentValueChanged: {
                    root.hours = hours.currentValue;
                }
            }
        }

        Item {
            width: minutes.width;
            height: parent.height;

            Text {
                id: minutesTitle;

                text: qsTr("Minutes");

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Spinner {
                id: minutes;

                anchors.top: minutesTitle.bottom;
                anchors.topMargin: 5;

                onCurrentValueChanged: {
                    root.minutes = minutes.currentValue;
                }
            }
        }
    }

    //    //icons
    //    property int iconWidth: 12;
    //    property int iconHeight: 10;
    //    property string iconUpSource: "../../../" + "Icons/" + Style.theme + "/" + "Up" + "_On_Normal.svg";
    //    property string iconDownSource: "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg";

    //    //buttons
    //    property int buttonWidth: 15;
    //    property int buttonHeight: 10;

    //    property string buttonColor:"";
    //    property int buttonHighlighted: -1;

    //    property int value: 0;
    //    property string type;

    //    TreeItemModel {
    //        id: intervalModel;

    //        Component.onCompleted: {
    //            let index = intervalModel.InsertNewItem();

    //            intervalModel.SetData("Id", "minute", index)
    //            intervalModel.SetData("Name", "minute", index)

    //            index = intervalModel.InsertNewItem();

    //            intervalModel.SetData("Id", "hour", index)
    //            intervalModel.SetData("Name", "hour", index)

    //            index = intervalModel.InsertNewItem();

    //            intervalModel.SetData("Id", "day", index)
    //            intervalModel.SetData("Name", "day", index)

    //            comboBox.model = intervalModel;
    //        }
    //    }

    //    RegExpValidator {
    //        id: validator;

    //        regExp: /^[1-9]\d*$/;
    //    }

    //    Row {
    //        height: root.height;

    //        CustomTextField {
    //            id: valueField;

    //            anchors.verticalCenter: parent.verticalCenter;

    //            height: 20;
    //            width: 30;

    //            textInputValidator: validator;

    //            text: root.value;

    //            horizontalAlignment: TextInput.AlignHCenter;

    //            Keys.onUpPressed: {
    //                upButton.clicked();
    //            }

    //            Keys.onDownPressed: {
    //                downButton.clicked();
    //            }

    //            MouseArea {
    //                anchors.fill: valueField;

    //                onWheel: {
    //                    if (wheel.angleDelta.y > 0){
    //                        upButton.clicked();
    //                    }
    //                    else{
    //                        downButton.clicked();
    //                    }
    //                }
    //            }
    //        }

    //        Column {
    //            anchors.verticalCenter: parent.verticalCenter;
    //            AuxButton {
    //                id: upButton;

    //                width: root.buttonWidth;
    //                height: root.buttonHeight;

    //                iconWidth: root.iconWidth;
    //                iconHeight: root.iconHeight;
    //                iconSource: root.iconUpSource;

    //                highlighted: root.buttonHighlighted >= 0 ? root.buttonHighlighted : containsMouse;
    //                color: root.buttonColor !== "" ? root.buttonColor : defaultColor;

    //                onClicked: {
    //                    root.value++;
    //                }
    //            }

    //            AuxButton {
    //                id: downButton;

    //                width: root.buttonWidth;
    //                height: root.buttonHeight;

    //                iconWidth: root.iconWidth;
    //                iconHeight: root.iconHeight;
    //                iconSource: root.iconDownSource;

    //                highlighted: root.buttonHighlighted >= 0 ? root.buttonHighlighted : containsMouse;
    //                color: root.buttonColor !== "" ? root.buttonColor : defaultColor;


    //                onClicked: {
    //                    if (root.value > 0){
    //                        root.value--;
    //                    }
    //                }
    //            }
    //        }

    //        ComboBox {
    //            id: comboBox;

    //            width: 70;
    //            height: parent.height;

    //            radius: 3;

    //            onCurrentIndexChanged: {
    //                if (comboBox.currentIndex >= 0){
    //                    let id = comboBox.model.GetData("Id", comboBox.currentIndex);
    //                    root.type = id;
    //                }
    //            }
    //        }
    //    }
}
