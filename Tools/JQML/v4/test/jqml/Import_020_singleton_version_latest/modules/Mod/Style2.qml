pragma Singleton
import QtQuick
QtObject {
    property string name: "style 2.0"
    property int value: 7
    function twice() {
        return value * 2
    }
}
