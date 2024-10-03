import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: root;

    width: 700;
    height: column.height;

    property int interval: model.Interval;
    property string time: model.StartTime;

    property var itemData: model;

    function secondsToDhms(seconds) {
        seconds = Number(seconds);
        var d = Math.floor(seconds / (3600*24));
        var h = Math.floor(seconds % (3600*24) / 3600);
        var m = Math.floor(seconds % 3600 / 60);

        return {'d': d, 'h': h, 'm': m};
    }

    Column {
        id: column;
        width: root.width;
        spacing: Style.size_mainMargin;

        GroupElementView {
            id: group;
            width: parent.width;

            ElementView {
                id: timePicker;
                name: qsTr("Start Time")
                controlComp: Component {
                    TimePicker{
                        id: timePicker;

                        onTimeChanged: {
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

                            let timeStr = "";
                            if (model.StartTime !== ""){
                                timeStr = model.StartTime.split(' ')[1];
                            }

                            let newTimeStr = datestring.split(' ')[1];
                            if (timeStr !== newTimeStr){
                                model.StartTime = datestring;
                            }
                        }

                        property bool ignoringChanging: false;

                        onAllCompletedChanged: {
                            if (timePicker.allCompleted){
                                if (model.StartTime !== ""){
                                    let dateTime = model.StartTime;

                                    let data = dateTime.split(' ')
                                    let date = data[0];
                                    let time = data[1]

                                    let hours = Number(time.split(':')[0])
                                    let minutes = Number(time.split(':')[1])

                                    timePicker.ignoringChanging = true;
                                    timePicker.setHours(hours);
                                    timePicker.ignoringChanging = false;

                                    timePicker.setMinutes(minutes);
                                }
                            }
                        }
                    }
                }
            }

            ComboBoxElementView {
                id: comboBox;
                name: qsTr("Backup Interval");
                onCurrentIndexChanged: {
                    if (comboBox.currentIndex >= 0){
                        let id = comboBox.model.getData("Id", comboBox.currentIndex);
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

                        if (newInterval !== -1 && root.itemData.Interval !== newInterval){
                            root.itemData.Interval = newInterval;
                        }
                    }
                }

                onModelChanged: {
                    let seconds = Number(root.itemData.Interval);
                    var d = Math.floor(seconds / (3600*24));

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

        TreeItemModel{
            id: comboBoxModel;

            Component.onCompleted: {

                let index = comboBoxModel.insertNewItem();
                comboBoxModel.setData("Id", "Day", index)
                comboBoxModel.setData("Name", qsTr("Day"), index)

                index = comboBoxModel.insertNewItem();
                comboBoxModel.setData("Id", "Week", index)
                comboBoxModel.setData("Name", qsTr("Week"), index)

                index = comboBoxModel.insertNewItem();
                comboBoxModel.setData("Id", "Month", index)
                comboBoxModel.setData("Name", qsTr("Month"), index)

                comboBox.model = comboBoxModel;
            }
        }
    }
}
