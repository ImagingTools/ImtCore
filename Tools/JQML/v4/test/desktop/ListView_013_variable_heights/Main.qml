import QtQuick

Item {
    id: root
    width: 120
    height: 200

    ListView {
        id: view
        width: 120
        height: 200
        spacing: 5
        model: 4
        delegate: Rectangle {
            width: view.width
            height: 20 + index * 15
            property int row: index
        }
    }

    Component.onCompleted: {
        const a = view.itemAtIndex(0)
        const b = view.itemAtIndex(1)
        const c = view.itemAtIndex(2)
        const d = view.itemAtIndex(3)
        console.log("heights", a.height, b.height, c.height, d.height)
        console.log("ys", a.y, b.y, c.y, d.y)
        console.log("gaps", b.y - (a.y + a.height), c.y - (b.y + b.height), d.y - (c.y + c.height))
        console.log("content", view.contentHeight)
        Qt.quit()
    }
}
