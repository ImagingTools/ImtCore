import QtQuick
Item {
    Box {
        id: box
        Item { objectName: "first" }
        Item { objectName: "second" }
    }
    Component.onCompleted: {
        console.log("children", box.slot.children.length, box.slot.children[0]?.objectName, box.slot.children[1]?.objectName)
        Qt.quit()
    }
}
