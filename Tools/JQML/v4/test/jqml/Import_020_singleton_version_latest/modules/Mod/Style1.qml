pragma Singleton
import QtQuick
QtObject {
    property string name: "style 1.0"
    property int value: 7
    function twice() {
        return value * 2
    }
}
