import QtQuick

Item {
    Component {
        id: comp
        Item {
            width: 90
            height: 55
        }
    }

    Loader {
        id: loader
        sourceComponent: comp

        onLoaded: {
            console.log("loader", width, height)
            console.log("item", item.width, item.height)
            Qt.quit()
        }
    }
}
