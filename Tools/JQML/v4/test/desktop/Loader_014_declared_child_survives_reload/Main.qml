import QtQuick 2.12

// What is declared inside a Loader (a Connections, a Timer) stays when the Loader loads again; the item is replaced.
Item {
    id: root
    width: 300
    height: 300
    property int pings: 0
    property int count: 0

    Loader {
        id: loader
        active: false
        Connections {
            target: root
            function onPingsChanged() { console.log("connection heard", root.pings) }
        }
        sourceComponent: Rectangle {
            id: box
            width: 40
            height: 40
            Component.onCompleted: console.log("completed", loader.item === null)
        }
    }

    Timer {
        interval: 20
        running: true
        onTriggered: {
            loader.active = true
            console.log("first load: item", loader.item !== null, "width", loader.width)
            root.pings = 1
            loader.active = false
            console.log("unloaded: item", loader.item === null)
            loader.active = true
            console.log("second load: item", loader.item !== null, "width", loader.width)
            root.pings = 2
            Qt.quit()
        }
    }
}
