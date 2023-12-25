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
}
