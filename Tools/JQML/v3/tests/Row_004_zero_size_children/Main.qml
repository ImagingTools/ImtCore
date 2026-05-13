import QtQuick

Item {
    Row {
        id: row
        spacing: 10
        padding: 5

        Item { width: 20; height: 10 }
        Item { width: 0; height: 0 }
        Item { width: 40; height: 12 }
    }

    Component.onCompleted: {
        // Row should include zero-size children in layout spacing
        // Width: 20 + 0 + 40 + 20(spacing) + 10(padding) = 90
        console.log("row", row.width, row.height)
        Qt.quit()
    }
}
