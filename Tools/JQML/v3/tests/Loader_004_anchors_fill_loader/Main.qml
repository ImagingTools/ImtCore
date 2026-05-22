import QtQuick

Item {
    width: 300
    height: 200

    Component {
        id: comp
        Item {
            width: 20
            height: 20
        }
    }

    Loader {
        id: loader
        anchors.fill: parent
        anchors.margins: 10
        sourceComponent: comp

        onLoaded: {
            console.log("loader", width, height)
            console.log("item", item.width, item.height)
            Qt.quit()
        }
    }
}
