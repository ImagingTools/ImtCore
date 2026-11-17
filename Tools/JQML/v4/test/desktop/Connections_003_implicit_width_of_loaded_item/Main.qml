import QtQuick

// A Connections whose target is the item of a Loader (the Connections is declared in the Loader, whose item replaces nothing of it)
// hears onImplicitWidthChanged when the item gets wider, as the filter chips of ImtCore tell their panel to lay out again.
Item {
    id: root
    width: 300
    height: 100
    property int hits: 0
    property int base: 0

    Loader {
        id: loader
        sourceComponent: Component { Chip {} }

        Connections {
            target: loader.item
            ignoreUnknownSignals: true
            function onImplicitWidthChanged() { root.hits++ }
        }
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            root.base = root.hits
            console.log("loaded", loader.item !== null, "status", loader.status)
            loader.item.value = "Defect"
            second.start()
        }
    }

    Timer {
        id: second
        interval: 50
        onTriggered: {
            console.log("heard the change", root.hits > root.base)
            loader.active = false
            loader.active = true
            third.start()
        }
    }

    Timer {
        id: third
        interval: 50
        onTriggered: {
            // the Connections survived the reload of the Loader and follows the new item
            var before = root.hits
            loader.item.value = "Defect"
            fourth.before = before
            fourth.start()
        }
    }

    Timer {
        id: fourth
        property int before: 0
        interval: 50
        onTriggered: {
            console.log("heard it after a reload", root.hits > before)
            Qt.quit()
        }
    }
}
