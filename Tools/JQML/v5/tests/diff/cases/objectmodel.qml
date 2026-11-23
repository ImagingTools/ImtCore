import QtQuick
import QtQml.Models

// ObjectModel: its items in a ListView and a Repeater (no delegate), the count, append / insert / move / remove, the
// positions of the items and their parents
Item {
    id: root
    width: 400
    height: 300
    ObjectModel {
        id: items
        Rectangle { width: 100; height: 30; objectName: "a" }
        Rectangle { width: 100; height: 40; objectName: "b" }
        Rectangle { width: 100; height: 50; objectName: "c" }
    }
    ObjectModel {
        id: repeated
        Rectangle { width: 20; height: 20; objectName: "x" }
        Rectangle { width: 30; height: 20; objectName: "y" }
    }
    ListView { id: list; width: 100; height: 100; model: items; clip: true }
    Row { id: row; y: 200; Repeater { id: repeater; model: repeated } }
    Component { id: extra; Rectangle { width: 100; height: 25 } }
    function names(view) {
        let out = []
        for (let i = 0; i < items.count; i++) {
            let it = items.get(i)
            out.push(it.objectName + "@" + it.y + (it.parent === list.contentItem ? "" : "!"))
        }
        return out.join(" ")
    }
    Component.onCompleted: {
        list.forceLayout()
        console.log("list", list.count, items.count, names(), list.contentHeight, list.currentIndex, list.currentItem.objectName)
        row.forceLayout()
        console.log("repeater", repeater.count, repeater.itemAt(1).objectName, repeater.itemAt(1).x, repeater.itemAt(0).parent === row)
        let d = extra.createObject(null, { objectName: "d" })
        items.append(d); list.forceLayout()
        console.log("append", list.count, names())
        items.move(0, 2); list.forceLayout()
        console.log("move", names())
        items.remove(1); list.forceLayout()
        console.log("remove", list.count, items.count, names())
        list.contentY = 60; list.forceLayout()
        console.log("scrolled", names(), items.get(0).visible)
        repeated.insert(0, extra.createObject(null, { objectName: "z", width: 5 })); row.forceLayout()
        console.log("repeater insert", repeater.count, repeater.itemAt(0).objectName, repeater.itemAt(1).x)
        Qt.quit()
    }
}
