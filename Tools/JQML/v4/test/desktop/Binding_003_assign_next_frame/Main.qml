import QtQuick

// Same write as Binding_002, observed after the binding pass that follows the script.
Item {
    id: root
    property int a: 10
    property int b: a + 5

    Timer {
        interval: 20
        running: true
        onTriggered: {
            console.log("b", root.b)
            Qt.quit()
        }
    }

    Component.onCompleted: a = 20
}
