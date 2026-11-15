import QtQuick

Item {
    id: root
    width: 220
    height: 80

    ListView {
        id: view
        width: 220
        height: 80
        orientation: Qt.Horizontal
        spacing: 10
        model: 3
        delegate: Rectangle {
            width: 50
            height: 60
        }
    }

    Component.onCompleted: {
        console.log("orient", view.orientation, Qt.Horizontal, ListView.Horizontal)
        console.log("content", view.contentWidth, view.contentHeight)
        const a = view.itemAtIndex(0)
        const b = view.itemAtIndex(1)
        const c = view.itemAtIndex(2)
        console.log("xs", a.x, b.x, c.x)
        console.log("step", b.x - a.x, c.x - b.x)
        Qt.quit()
    }
}
