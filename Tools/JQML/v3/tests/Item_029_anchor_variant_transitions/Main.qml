import QtQuick

Item {
    id: root
    width: 600
    height: 400

    Rectangle {
        id: rect
        width: 150
        height: 80

        Component.onCompleted: {
            anchors.left = root.left
            anchors.top = root.top
            console.log("step1", x, y)
            anchors.left = undefined
            anchors.right = root.right
            console.log("step2", x, width)
            anchors.right = undefined
            anchors.horizontalCenter = root.horizontalCenter
            console.log("step3", x)
            anchors.top = undefined
            anchors.bottom = root.bottom
            console.log("step4", y)
            anchors.bottom = undefined
            anchors.verticalCenter = root.verticalCenter
            console.log("step5", y)
            console.log("final", width, height)
            Qt.quit()
        }
    }
}
