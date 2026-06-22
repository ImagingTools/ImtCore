import QtQuick

Item {
    Loader {
        id: loader

        Component.onCompleted: {
            setSource("LoadedItem.qml")
        }

        onLoaded: {
            console.log("loader016", item.token, item.label)
            Qt.quit()
        }
    }
}
