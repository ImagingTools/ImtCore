import QtQuick

Item {
    Column {
        id: column
        spacing: 10
        padding: 5

        Item { width: 20; height: 10 }
        Item { width: 30; height: 15; visible: false }
        Item { width: 40; height: 12 }
    }

    Component.onCompleted: {
        // Column should exclude invisible children from layout
        // Height: 10 + 12 + 10(spacing) + 10(padding) = 42
        console.log("column", column.width, column.height)
        Qt.quit()
    }
}
