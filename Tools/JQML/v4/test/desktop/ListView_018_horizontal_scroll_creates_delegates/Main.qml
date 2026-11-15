import QtQuick

Item {
    width: 500
    height: 100
    Component.onCompleted: {
        list.contentX = 1000
        console.log("after scroll, contentX", list.contentX)
        console.log("delegate 10 exists", list.itemAtIndex(10) !== null, "delegate 0 exists", list.itemAtIndex(0) !== null)
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
        }
    }
}
