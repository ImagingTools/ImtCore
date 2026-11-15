import QtQuick

Item {
    id: root
    property alias boxWidth: box.width

    Rectangle {
        id: box
        width: 40
        height: 10
    }

    Component.onCompleted: {
        console.log("width", root.boxWidth)
        root.boxWidth = 55
        console.log("assigned", box.width)
        Qt.quit()
    }
}
