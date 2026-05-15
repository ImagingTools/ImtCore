pragma Singleton
import QtQuick

QtObject {
    property int counter: 0
    property string label: "init"

    function mark(tag) {
        counter += 1
        label = tag + ":" + counter
    }
}
