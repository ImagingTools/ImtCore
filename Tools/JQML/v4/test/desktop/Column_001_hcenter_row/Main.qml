import QtQuick

Item {
    id: root
    width: 400
    height: 200

    Column {
        id: column
        anchors.centerIn: parent
        width: parent.width
        spacing: 10

        Row {
            id: row
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 10
            Rectangle { width: 40; height: 20; color: "red" }
            Rectangle { width: 40; height: 20; color: "blue" }
        }
    }

    Component.onCompleted: {
        console.log("rowx", row.x)
        console.log("center", (column.width - row.width) / 2)
        Qt.quit()
    }
}
