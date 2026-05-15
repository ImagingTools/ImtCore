pragma Singleton
import QtQuick

QtObject {
    property int count: 0
    property string last: "none"

    function reset() {
        count = 0
        last = "none"
    }

    function bump(tag, delta) {
        count += delta
        last = tag
    }
}
