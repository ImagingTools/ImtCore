import QtQuick

Item {
    id: root

    property int seq: 0

    Item {
        id: childA

        Item {
            id: childA1
            Component.onCompleted: {
                root.seq = root.seq + 1
                console.log(root.seq + ":childA1")
                if (root.seq === 7) Qt.quit()
            }
        }

        Item {
            id: childA2
            Component.onCompleted: {
                root.seq = root.seq + 1
                console.log(root.seq + ":childA2")
                if (root.seq === 7) Qt.quit()
            }
        }

        Component.onCompleted: {
            root.seq = root.seq + 1
            console.log(root.seq + ":childA")
            if (root.seq === 7) Qt.quit()
        }
    }

    Item {
        id: childB

        Item {
            id: childB1
            Component.onCompleted: {
                root.seq = root.seq + 1
                console.log(root.seq + ":childB1")
                if (root.seq === 7) Qt.quit()
            }
        }

        Item {
            id: childB2
            Component.onCompleted: {
                root.seq = root.seq + 1
                console.log(root.seq + ":childB2")
                if (root.seq === 7) Qt.quit()
            }
        }

        Component.onCompleted: {
            root.seq = root.seq + 1
            console.log(root.seq + ":childB")
            if (root.seq === 7) Qt.quit()
        }
    }

    Component.onCompleted: {
        root.seq = root.seq + 1
        console.log(root.seq + ":root")
        if (root.seq === 7) Qt.quit()
    }
}
