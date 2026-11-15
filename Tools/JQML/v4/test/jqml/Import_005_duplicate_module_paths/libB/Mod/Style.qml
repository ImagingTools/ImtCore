pragma Singleton
import QtQuick
QtObject {
    property string name: "B style"
    property int value: 7
    function twice() {
        return value * 2
    }
}
