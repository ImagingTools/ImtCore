import QtQuick 2.12

// A Loader made by a Repeater while running, with a property that follows the window and a handler of its change that moves the Loader
// once it has an item, and gets its item from a binding that comes before it (the dialogs of the application do this). In Qt the first value of the binding is set before the item is loaded.
Item {
    id: root
    width: 300
    height: 200

    property Component dialogComponent: Component { Rectangle { width: 50; height: 20 } }

    ListModel { id: dialogs }

    Repeater {
        id: repeater
        model: dialogs
        delegate: Item {
            Loader {
                id: dialogLoader
                sourceComponent: root.dialogComponent
                property real mainWidth: root.width
                property real mainWidthPrev: 0
                onMainWidthChanged: {
                    console.log("handler ran", mainWidth, "item", dialogLoader.item !== null)
                    if (dialogLoader.item) {
                        dialogLoader.x += mainWidth - mainWidthPrev
                        mainWidthPrev = mainWidth
                    }
                }
                onLoaded: console.log("loaded at", dialogLoader.x, dialogLoader.y)
            }
        }
    }

    Timer {
        interval: 20
        running: true
        onTriggered: {
            dialogs.append({ name: "menu" })
            check.start()
        }
    }
    Timer {
        id: check
        interval: 60
        onTriggered: {
            const loader = repeater.itemAt(0).children[0]
            console.log("loader at", loader.x, loader.y)
            Qt.quit()
        }
    }
}
