import QtQuick

Item {
    Component {
        id: comp
        Item {
            width: 40
            height: 30
        }
    }

    Loader {
        id: loader
        width: 120
        height: 80
        sourceComponent: comp

        onLoaded: {
            console.log("loader", width, height)
            console.log("item", item.width, item.height)
            Qt.quit()
        }
    }
}
