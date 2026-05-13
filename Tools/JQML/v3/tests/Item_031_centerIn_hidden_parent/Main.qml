import QtQuick

Item {
    width: 300
    height: 200

    Rectangle {
        id: rectA
        width: 150
        height: 80
        visible: false
        anchors.centerIn: parent

        Rectangle {
            id: rectB
            width: 100
            height: 60
            anchors.centerIn: parent
        }
    }

    Component.onCompleted: {
        console.log("rectA", rectA.x, rectA.y, rectA.visible)
        console.log("rectB", rectB.x, rectB.y, rectB.visible)
        Qt.quit()
    }
}
