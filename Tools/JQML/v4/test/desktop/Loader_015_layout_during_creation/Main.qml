import QtQuick 2.12

// A Column of a title, a Loader and a border. The item of the Loader reads geometry in Component.onCompleted, which lays the scene out
// while the item is made: the Loader has no size then. The Column must still place the border under the item once it is loaded.
Item {
    id: root
    width: 300
    height: 300

    Component {
        id: body
        Column {
            spacing: 5
            Rectangle {
                id: first
                width: 100
                height: 20
                color: "red"
                Component.onCompleted: console.log("completed, y", first.y, "parent height", first.parent.height)
            }
            Rectangle { width: 100; height: 30; color: "blue" }
        }
    }

    Column {
        id: column
        spacing: 10
        Text { text: "title"; font.pixelSize: 16 }
        Loader { id: loader; sourceComponent: body }
        Rectangle { width: 100; height: 1; color: "black" }
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            console.log("column", column.height, "loader", loader.y, loader.height, "border", column.children[2].y)
            Qt.quit()
        }
    }
}
