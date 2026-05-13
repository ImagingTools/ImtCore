import QtQuick

Item {
    Column {
        id: column
        spacing: 10
        padding: 5

        Item { width: 20; height: 10 }
        Item { width: 0; height: 0 }
        Item { width: 40; height: 12 }
    }

    Component.onCompleted: {
        // Column should include zero-size children in layout spacing
        // Height: 10 + 0 + 12 + 20(spacing) + 10(padding) = 52
        console.log("column", column.width, column.height)
        Qt.quit()
    }
}
