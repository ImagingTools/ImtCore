import QtQuick

// Horizontal ListView lays out delegates by each item's width, not a uniform step.
Item {
    id: root
    width: 400
    height: 80

    ListView {
        id: view
        width: 400
        height: 80
        orientation: ListView.Horizontal
        spacing: 4
        model: 4
        delegate: Rectangle {
            width: 30 + index * 20
            height: view.height
        }
    }

    Component.onCompleted: {
        const a = view.itemAtIndex(0)
        const b = view.itemAtIndex(1)
        const c = view.itemAtIndex(2)
        const d = view.itemAtIndex(3)
        console.log("widths", a.width, b.width, c.width, d.width)
        console.log("xs", a.x, b.x, c.x, d.x)
        console.log("gaps", b.x - (a.x + a.width), c.x - (b.x + b.width), d.x - (c.x + c.width))
        console.log("content", view.contentWidth)
        Qt.quit()
    }
}
