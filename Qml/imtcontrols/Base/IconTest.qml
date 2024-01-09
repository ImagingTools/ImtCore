pragma Singleton
import QtQuick 2.12
//import QtQuick.Controls 2.15

QtObject {
    id: iconEnum
    property QtObject mode: QtObject {
        property string normal: "Normal"
        property string disabled: "Disabled"
        property string active: "Active"
        property string selected: "Selected"
    }

    property QtObject state: QtObject {
        property string on: "On"
        property string off: "Off"
    }

}


