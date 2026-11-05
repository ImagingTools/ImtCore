import QtQuick

Item {
    id: root
    width: 230
    height: 230

    GridView {
        id: view
        width: 230
        height: 230
        cellWidth: 110
        cellHeight: 110
        model: 4
        delegate: Rectangle {
            width: 100
            height: 100
        }
    }

    Component.onCompleted: {
        console.log("count", view.count)
        console.log("content", view.contentWidth, view.contentHeight)
        const a = view.itemAtIndex(0)
        const b = view.itemAtIndex(1)
        const c = view.itemAtIndex(2)
        console.log("pos", a.x, a.y, b.x, b.y, c.x, c.y)
        Qt.quit()
    }
}
