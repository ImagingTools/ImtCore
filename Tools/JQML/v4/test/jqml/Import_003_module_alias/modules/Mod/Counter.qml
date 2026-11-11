pragma Singleton
import QtQuick
QtObject {
    property string name: "counter"
    property int value: 5
    function twice() {
        return value * 2
    }
}
