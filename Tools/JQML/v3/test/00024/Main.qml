import QtQuick

Item {
    id: root
    width: 300
    height: 200

    Rectangle {
        id: child
        anchors.fill: parent
        color: "purple"

        Component.onCompleted: {
            console.log("child.width (начальное):", width)
            console.log("child.height (начальное):", height)
        }
    }

    Timer {
        interval: 500; running: true; repeat: false
        onTriggered: {
            root.width = 500
            root.height = 400
            console.log("root.width (изменено):", root.width)
            console.log("root.height (изменено):", root.height)
            console.log("child.width (после изменения родителя):", child.width)
            console.log("child.height (после изменения родителя):", child.height)
        }
    }
}