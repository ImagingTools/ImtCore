import QtQuick

Item {
    id: root
    property int seq: 0

    Component {
        id: comp
        Item {
            width: 11
            height: 22
        }
    }

    Loader {
        id: loader

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
            console.log(root.seq + ":completed")
            sourceComponent = comp
        }
    }
}
