import QtQuick

Item {
    id: root
    property int seq: 0

    Loader {
        id: loader
        sourceComponent: Item {
            property string message: "dynamic-item"

            Component.onCompleted: {
                root.seq = root.seq + 1
                console.log(root.seq + ":dynamicCompleted")
            }
        }

        onStatusChanged: {
            if (status === Loader.Ready) {
                root.seq = root.seq + 1
                console.log(root.seq + ":ready " + loader.item.message)
            }
        }

        onItemChanged: {
            root.seq = root.seq + 1
            console.log(root.seq + ":itemChanged")
        }

        onLoaded: {
            root.seq = root.seq + 1
            console.log(root.seq + ":loaded")
            Qt.quit()
        }

        Component.onCompleted: {
            root.seq = root.seq + 1
            console.log(root.seq + ":loaderCompleted")
        }
    }
}
