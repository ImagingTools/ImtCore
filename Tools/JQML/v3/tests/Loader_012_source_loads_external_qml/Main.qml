import QtQuick

Item {
    Loader {
        id: loader
        width: 120
        height: 80
        source: "LoadedItem.qml"

        onLoaded: {
            console.log("loader", width, height)
            console.log("item", item.width, item.height)
            item.width = 25
            console.log("itemAfter", item.width, item.height)
            Qt.quit()
        }
    }
}
