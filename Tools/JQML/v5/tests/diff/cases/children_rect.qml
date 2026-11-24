import QtQuick

// Item.childrenRect (QQuickContents): all the children, hidden ones too; a moved child computes it again, an added one
// only widens it (ListItemSetting of QmlAppTemplate: implicitHeight: childrenRect.height)
Item {
    id: root
    width: 300; height: 200
    Item {
        id: box
        implicitWidth: childrenRect.width
        implicitHeight: childrenRect.height
        onChildrenRectChanged: console.log("changed", childrenRect.x, childrenRect.y, childrenRect.width, childrenRect.height)
        Rectangle { id: a; x: 10; y: 5; width: 30; height: 20 }
        Rectangle { id: b; x: 50; y: 40; width: 20; height: 10; visible: false }
    }
    Item { id: empty }
    Timer { id: after; interval: 50; onTriggered: { console.log(box.childrenRect.x, box.childrenRect.height); Qt.quit() } }
    Component { id: comp; Rectangle { x: -5; y: 100; width: 10; height: 10 } }
    Component.onCompleted: {
        let r = box.childrenRect
        console.log(r.x, r.y, r.width, r.height, box.implicitWidth, box.implicitHeight, empty.childrenRect.width, empty.childrenRect.x)
        b.x = 80
        console.log(box.childrenRect.width)
        b.x = 20
        console.log(box.childrenRect.width, box.childrenRect.x)
        let c = comp.createObject(box)
        console.log(box.childrenRect.x, box.childrenRect.y, box.childrenRect.width, box.childrenRect.height)
        c.destroy()
        after.start()
    }
}
