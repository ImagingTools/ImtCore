import QtQuick

// Component.onDestruction when a Loader unloads its item: the objects made as values of its properties (a
// QtObject, a list<QtObject>) first, the last declared first, then the children (a child after its own children), the
// item last; an object given as the value of a property is owned by its item and goes with it

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
            Item {
                Component.onDestruction: console.log("child destroyed")
                Item {
                    Component.onDestruction: console.log("grandchild destroyed")
                }
            }
            Item {
                Component.onDestruction: console.log("second child destroyed")
            }
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
