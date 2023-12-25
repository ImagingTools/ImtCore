import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: root;

    width: 150;
    height: column.height;

    property int interval: model.Interval;
    property string time: model.StartTime;

    property var itemData: model;

    function secondsToDhms(seconds) {
        seconds = Number(seconds);
        var d = Math.floor(seconds / (3600*24));
        var h = Math.floor(seconds % (3600*24) / 3600);
        var m = Math.floor(seconds % 3600 / 60);

        console.log("d", d);
        console.log("h", h);
        console.log("m", m);

        return {'d': d, 'h': h, 'm': m};
    }

    Column {
        id: column;

        width: root.width;

        spacing: 7;

        Text {
            text: "Start Time";

            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;
        }

        TimePicker{
            id: timePicker;

            onTimeChanged: {
                console.log("onTimeChanged");
                if (timePicker.ignoringChanging){
                    return;
                }

                let hours = timePicker.hours;
                let minutes = timePicker.minutes;

                var d = new Date();

                d.setHours(hours);
                d.setMinutes(minutes);

                var datestring = ("0" + d.getDate()).slice(-2) + "-" + ("0"+(d.getMonth()+1)).slice(-2) + "-" +
                        d.getFullYear() + " " + ("0" + d.getHours()).slice(-2) + ":" + ("0" + d.getMinutes()).slice(-2);

                console.log("model.StartTime", model.StartTime);
                console.log("datestring", datestring);

                let timeStr = "";
                if (model.StartTime !== ""){
                    timeStr = model.StartTime.split(' ')[1];
                }

                let newTimeStr = datestring.split(' ')[1];
                if (timeStr !== newTimeStr){
                    console.log("model.StartTime = ", datestring);
                    model.StartTime = datestring;
                }
            }

            property bool ignoringChanging: false;

            onAllCompletedChanged: {
                if (timePicker.allCompleted){
                    console.log("onAllCompletedChanged");
                    if (model.StartTime !== ""){
                        let dateTime = model.StartTime;

                        let data = dateTime.split(' ')
                        let date = data[0];
                        let time = data[1]

                        let hours = Number(time.split(':')[0])
                        let minutes = Number(time.split(':')[1])

                        console.log("hours");
                        console.log("minutes");

                        timePicker.ignoringChanging = true;
                        timePicker.setHours(hours);
                        timePicker.ignoringChanging = false;

                        timePicker.setMinutes(minutes);
                    }
                }
            }
        }

        Text {
            text: "Backup Interval";

            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;
        }

        TreeItemModel{
            id: comboBoxModel;

            Component.onCompleted: {

                let index = comboBoxModel.InsertNewItem();
                comboBoxModel.SetData("Id", "Day", index)
                comboBoxModel.SetData("Name", "Day", index)

                index = comboBoxModel.InsertNewItem();
                comboBoxModel.SetData("Id", "Week", index)
                comboBoxModel.SetData("Name", "Week", index)

                index = comboBoxModel.InsertNewItem();
                comboBoxModel.SetData("Id", "Month", index)
                comboBoxModel.SetData("Name", "Month", index)

                comboBox.model = comboBoxModel;
            }
        }

        ComboBox {
            id: comboBox;

            width: parent.width;
            height: 25;

            radius: 3;

            textCentered: false;

            onCurrentIndexChanged: {
                console.log("onCurrentIndexChanged", itemData.Interval);
                if (comboBox.currentIndex >= 0){
                    let id = comboBox.model.GetData("Id", comboBox.currentIndex);
                    let secondsInHour = 3600;

                    let newInterval = -1;
                    if (id === "Day"){
                        newInterval = 24 * secondsInHour;
                    }
                    else if (id === "Week"){
                        newInterval = 24 * secondsInHour * 7;
                    }
                    else if (id === "Month"){
                        newInterval = 24 * secondsInHour * 30;
                    }

                    console.log("itemData.Interval", itemData.Interval);
                    console.log("newInterval", newInterval);

                    if (newInterval !== -1 && itemData.Interval !== newInterval){
                        console.log("itemData.Interval = ", newInterval);
                        itemData.Interval = newInterval;
                    }
                }
            }

            onModelChanged: {
                let seconds = Number(itemData.Interval);
                console.log("seconds", seconds);
                var d = Math.floor(seconds / (3600*24));
                console.log("d", d);

                if (d === 1){
                    comboBox.currentIndex = 0;
                }
                else if (d === 7){
                    comboBox.currentIndex = 1;
                }
                else if (d === 30){
                    comboBox.currentIndex = 2;
                }
            }
        }
    }
}
