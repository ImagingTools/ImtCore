import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import QtQuick.Dialogs 1.3

Item {
    id: root;

    width: 270;
    height: 30;

    property string value: model.Value;

    onValueChanged: {
        if (root.value !== timePicker.toStr()){
            timePicker.fromString(root.value);
        }
    }

    TimePicker {
        id: timePicker;

        width: 100;
        height: 20;

        onTimeChanged: {
            model.Value = timePicker.toStr();
        }
    }
}
