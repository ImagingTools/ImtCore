import QtQuick

Item {
    Row {
        id: row
        spacing: 10
        padding: 5

        Item { width: 20; height: 10 }
        Item { width: 30; height: 15; visible: false }
        Item { width: 40; height: 12 }
    }

    Component.onCompleted: {
        // Row should exclude invisible children from layout
        // Width: 20 + 40 + 10(spacing) + 10(padding) = 80
        console.log("row", row.width, row.height)
        Qt.quit()
    }
}
