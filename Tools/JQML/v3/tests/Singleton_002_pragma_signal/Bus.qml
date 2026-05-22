pragma Singleton
import QtQuick

QtObject {
    property int total: 0
    signal emitted(string tag, int value)

    function send(tag, value) {
        total += value
        emitted(tag, total)
    }
}
