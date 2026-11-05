import QtQuick

Item {
    id: root
    width: 100
    height: 40

    Repeater {
        model: 4
        Item {
            Component.onCompleted: console.log("row", index)
        }
    }

    Component.onCompleted: {
        console.log("done")
        Qt.quit()
    }
}
