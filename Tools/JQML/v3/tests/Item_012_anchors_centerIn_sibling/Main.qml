import QtQuick

Item {
    width: 300
    height: 300

    Item {
        id: ref
        x: 50
        y: 70
        width: 120
        height: 80
    }

    Item {
        id: box
        width: 20
        height: 10
        anchors.centerIn: ref
    }

    Component.onCompleted: {
        console.log("centerIn1", box.x, box.y)
        ref.x = 80
        ref.y = 100
        ref.width = 140
        ref.height = 100
        console.log("centerIn2", box.x, box.y)
        Qt.quit()
    }
}
