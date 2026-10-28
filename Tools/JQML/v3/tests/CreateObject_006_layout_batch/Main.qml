import QtQuick

Item {
    id: root

    Column {
        id: col
        spacing: 0
        Item {
            width: 10
            height: 10
        }
    }

    Component {
        id: blockComponent
        Item {
        }
    }

    Component.onCompleted: {
        var block = blockComponent.createObject(col)
        block.width = 10
        block.height = 15
        console.log("props " + block.width + " " + block.height)
        console.log("inside " + col.width + " " + col.height)
        Qt.quit()
    }
}
