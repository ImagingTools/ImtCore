import QtQuick

Item {
    id: root
    width: 300
    height: 200

    Rectangle {
        id: placeholder
        anchors.fill: parent
    }

    Timer {
        id: doneTimer
        interval: 1
        repeat: false
        onTriggered: {
            console.log("placeholder", placeholder.width, placeholder.height)
            console.log("item visible", loader.item.visible)
            Qt.quit()
        }
    }

    Loader {
        id: loader
        sourceComponent: Rectangle {
            width: root.width
            height: root.height
        }

        onLoaded: {
            console.log("loaded", item.visible)
            item.visible = false
            doneTimer.start()
        }
    }
}
