import QtQuick

// An object given as the value of a property (`property QtObject helper: QtObject { ... }`) is owned by
// its item: unloading the item destroys it too. Its bindings used to outlive the page and keep the
// whole unloaded subtree in memory.
Item {
    width: 100
    height: 100

    Loader {
        id: loader
        sourceComponent: page
    }

    Component {
        id: page
        Item {
            id: root
            property bool flag: true
            property QtObject helper: QtObject {
                property bool mirror: root.flag
                Component.onDestruction: console.log("helper destroyed", mirror)
            }
            property list<QtObject> extras: [
                QtObject { Component.onDestruction: console.log("extra destroyed") }
            ]
            Component.onDestruction: console.log("page destroyed")
        }
    }

    Timer {
        interval: 1
        running: true
        onTriggered: {
            console.log("loaded", loader.item !== null, loader.item.helper.mirror)
            loader.sourceComponent = null
            quitTimer.start()
        }
    }

    Timer {
        id: quitTimer
        interval: 50
        onTriggered: {
            console.log("unloaded", loader.item === null)
            Qt.quit()
        }
    }
}
