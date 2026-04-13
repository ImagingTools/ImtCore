import QtQuick

Item {
    Loader {
        id: loader
        source: "DynamicItem.qml"
        onStatusChanged: {
            if (status === Loader.Ready)
                console.log("Загружено:", loader.item.message)
        }
        onItemChanged: {
            console.log("onItemChanged")
        }
        onLoaded: {
            console.log("onLoaded")
        }
        Component.onCompleted: {
            console.log("Loader.onCompleted")
        }
    }
}