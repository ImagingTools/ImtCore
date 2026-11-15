import QtQuick

Item {
    width: 500
    height: 100
    Component.onCompleted: {
        console.log("root completed, count", list.count)
        Qt.quit()
    }

    ListView {
        id: list
        orientation: ListView.Horizontal
        width: parent.width
        height: parent.height
        model: 30
        delegate: Item {
            width: 100
            height: list.height
            Component.onCompleted: console.log("delegate", index)
        }
    }
}
