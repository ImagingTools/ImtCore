import QtQuick

// A use-site write through an alias must replace the target's own binding.
Item {
    id: root

    Item {
        id: host
        property alias chosen: loader.sourceComponent

        Loader {
            id: loader
            width: 20
            height: 20
            sourceComponent: defaultComp
        }

        Component {
            id: defaultComp
            Rectangle {
                objectName: "default"
                width: 10
                height: 10
            }
        }
    }

    Component {
        id: overrideComp
        Rectangle {
            objectName: "override"
            width: 10
            height: 10
        }
    }

    Component.onCompleted: {
        host.chosen = overrideComp
        console.log("name", loader.item.objectName)
        Qt.quit()
    }
}
