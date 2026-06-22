import QtQuick

Item {
    Loader {
        id: loader
        active: false

        Component.onCompleted: {
            setSource("LoadedItemA.qml", { token: 77 })
            source = "LoadedItemB.qml"
            active = true
        }

        onLoaded: {
            console.log("loader014", item.token)
            Qt.quit()
        }
    }
}
