import QtQuick

// What the bindings of the item of a Loader read from the Loader once the item is made: loader.item, loader.width, loader.height and
// loader.status, read at once after the load and later, and a Column and a section that are placed by the Loader once it has settled. The
// moment at which a layout runs is not compared (Qt puts a Column in order later, the engine in its next pass), only what is the same
// when it has. The same file runs with desktop Qt and in the browser.
Item {
    id: root
    width: 300
    height: 300

    Column {
        id: column
        spacing: 5

        Loader {
            id: loader
            active: false
            sourceComponent: Component {
                Rectangle {
                    id: box
                    width: 120
                    height: 40
                    color: "#80c0ff"
                    property bool boundItem: loader.item !== null
                    property int boundWidth: loader.width
                    property int boundHeight: loader.height
                    property int boundStatus: loader.status
                    property bool done: false
                    // an item with an onCompleted: the bindings of the item are evaluated before it, while the Loader has no item yet
                    Component.onCompleted: {
                        box.done = true
                        // Qt gives the item to the Loader after the onCompleted of the item
                        console.log("completed: item", loader.item !== null, "width", loader.width, "height", loader.height, "status", loader.status)
                    }
                }
            }
        }

        Rectangle {
            id: next
            width: 50
            height: 10
            color: "#ff8080"
        }
    }

    // a section that is placed by the height of the Loader without a layout
    Rectangle {
        id: below
        y: loader.height + 100
        width: 50
        height: 10
        color: "#80ff80"
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            loader.active = true
            // read at once, when the load has just been done
            console.log("at once: item", loader.item !== null, "width", loader.width, "height", loader.height, "status", loader.status)
            console.log("at once: item sees", loader.item.boundItem, loader.item.boundWidth, loader.item.boundHeight, loader.item.boundStatus)
            later.start()
        }
    }

    Timer {
        id: later
        interval: 50
        onTriggered: {
            console.log("later: next.y", next.y, "below.y", below.y, "item sees", loader.item.boundItem, loader.item.boundWidth, loader.item.boundHeight, loader.item.boundStatus)
            Qt.quit()
        }
    }
}
