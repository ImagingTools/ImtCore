import QtQuick
import "."

Item {
    id: root
    property int seen: 0

    Connections {
        target: Bus
        function onEmitted(tag, value) {
            root.seen += 1
            console.log("singleton2-signal", tag, value, root.seen)
        }
    }

    Component.onCompleted: {
        Bus.send("x", 2)
        Bus.send("y", 5)
        console.log("singleton2-final", Bus.total, root.seen)
        Qt.quit()
    }
}
