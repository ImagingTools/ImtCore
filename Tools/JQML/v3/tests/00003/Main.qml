import QtQuick

Item {
    Item {
        id: item
        signal mySignal(string msg)
        Component.onCompleted: {
            mySignal("First call")
            mySignal("Second call")
        }
    }
    Connections {
        target: item
        function onMySignal(msg) {
            console.log("Received:", msg)
        }
    }

    Component.onCompleted: {
        Qt.quit()
    }
}