import QtQuick

Item {
    width: 400
    height: 300

    Flickable {
        id: fl
        width: 200
        height: 100
        contentWidth: 600
        contentHeight: 300

        Rectangle { id: first; width: 50; height: 50 }
        Rectangle { id: second; x: 60; width: 50; height: 50 }
        Timer { id: timer }
    }

    Component.onCompleted: {
        console.log("children", fl.children.length)
        console.log("content children", fl.children[0].children.length)
        console.log("parent is content", first.parent === fl.contentItem, fl.children[0] === fl.contentItem)
        console.log("content size", fl.contentItem.width, fl.contentItem.height)
        console.log("first", first.x, first.y, "second", second.x, second.y)
        fl.contentX = 40
        console.log("mapped", second.mapToItem(null, 0, 0).x, second.mapToItem(fl.contentItem, 0, 0).x)
        Qt.quit()
    }
}
