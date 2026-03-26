import QtQuick

Item {
    Loader {
        id: loader
        source: "DynamicItem.qml"
        onStatusChanged: {
            if (status === Loader.Ready)
                console.log("Загружено:", loader.item.message)
        }
    }
}