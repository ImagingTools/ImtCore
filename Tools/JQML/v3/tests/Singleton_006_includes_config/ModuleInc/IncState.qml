pragma Singleton
import QtQuick

QtObject {
    property int value: 0
    property string trace: "-"

    function reset() {
        value = 0
        trace = "-"
    }

    function add(tag, delta) {
        value += delta
        trace = trace + "|" + tag + ":" + value
    }
}
