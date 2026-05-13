import QtQuick

Item {
    Column {
        id: col
        spacing: 4
        padding: 3

        Item { width: 20; height: 10 }
        Item { width: 40; height: 15 }
        Item { width: 10; height: 5 }
    }

    Component.onCompleted: {
        console.log("column", col.width, col.height)
        Qt.quit()
    }
}
