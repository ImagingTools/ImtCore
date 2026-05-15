import QtQuick

Item {
    Component {
        id: comp
        Item {
            anchors.fill: parent
            anchors.margins: 5
        }
    }

    Loader {
        id: loader
        width: 150
        height: 90
        sourceComponent: comp

        onLoaded: {
            console.log("loader", width, height)
            console.log("item", item.x, item.y)
            Qt.quit()
        }
    }
}
