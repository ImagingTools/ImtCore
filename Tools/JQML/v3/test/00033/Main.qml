import QtQuick

Item {
    Loader {
        id: loader
        sourceComponent: Item {
            property string message: "Динамический элемент"
            
            Component.onCompleted: {
                console.log("DynamicItem.onCompleted")
            }
        }
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