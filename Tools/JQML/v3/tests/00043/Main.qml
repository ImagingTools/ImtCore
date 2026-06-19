import QtQuick

Item {
    id: root
    width: 200
    height: 200

    Container1 {
        id: container

        Rectangle {
            width: 15
            height: 15
            color: "gray"
            onParentChanged: console.log("gray.onParentChanged", parent.objectName)
        }

        Rectangle {
            width: 15
            height: 15
            color: "yellow"
            onParentChanged: console.log("yellow.onParentChanged", parent.objectName)
        }

        Rectangle {
            width: 15
            height: 15
            color: "pink"
            onParentChanged: console.log("pink.onParentChanged", parent.objectName)
        }
    }

    Component.onCompleted: {
        Qt.quit()
    }
}
