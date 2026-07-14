import QtQuick

Item {
    Loader {
        id: loader
        active: false

        Component.onCompleted: {
            setSource("LoadedItem.qml", { token: 77, label: "fromSetSource" })
            active = true
        }

        onLoaded: {
            console.log("loader013", item.token, item.label)
            Qt.quit()
        }
    }
}
