import QtQuick

Item {
    Component {
        id: comp
        Item {
            width: 40
            height: 70
        }
    }

    Loader {
        id: loader
        width: 150
        sourceComponent: comp

        onLoaded: {
            console.log("loader", width, height)
            console.log("item", item.width, item.height)
            Qt.quit()
        }
    }
}
