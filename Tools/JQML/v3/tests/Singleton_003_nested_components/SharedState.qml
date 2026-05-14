pragma Singleton
import QtQuick

QtObject {
    property int value: 0
    property string history: ""

    function reset() {
        value = 0
        history = ""
    }

    function add(tag, delta) {
        value += delta
        history = history + "|" + tag + ":" + value
    }
}
