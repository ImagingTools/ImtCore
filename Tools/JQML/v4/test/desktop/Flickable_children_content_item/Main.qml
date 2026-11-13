import QtQuick

// The visual children of a Flickable are in its contentItem, which is the only entry of `children`: items declared in it, and items
// that get the Flickable as `parent`, are reparented to the contentItem, whose position is the scroll position and whose size is the content
// size. GridView is the same. The lines say where things are.
Item {
    id: root
    width: 300
    height: 300

    Flickable {
        id: flick
        width: 100
        height: 80
        contentWidth: 300
        contentHeight: 400

        Rectangle {
            id: first
            width: 20
            height: 20
        }
        Rectangle {
            id: second
            x: 30
            y: 40
            width: 20
            height: 20
        }
        Timer {
            id: timer
        }
        Item {
            id: hidden
            visible: false
        }
    }

    Component {
        id: itemComp
        Item {
            objectName: "made"
        }
    }

    GridView {
        id: grid
        y: 100
        width: 100
        height: 80
        cellWidth: 50
        cellHeight: 40
        model: 3
        delegate: Rectangle {
            width: 40
            height: 30
        }
    }

    Component.onCompleted: {
        console.log("flick children", flick.children.length, flick.children[0] === flick.contentItem)
        var content = flick.contentItem
        console.log("content children", content.children.length, content.children[0] === first, content.children[1] === second, content.children[2] === hidden)
        console.log("parents", first.parent === content, content.parent === flick, timer.parent === flick, hidden.parent === content)
        console.log("content size", content.width, content.height, content.x, content.y)
        flick.contentX = 30
        flick.contentY = 20
        console.log("content moved", content.x, content.y)
        flick.contentWidth = 250
        flick.contentHeight = 500
        console.log("content resized", content.width, content.height)

        var made = itemComp.createObject(flick)
        console.log("created with parent flick: parent is flick", made.parent === flick, "is content", made.parent === content)
        console.log("after create", flick.children.length, content.children.length)
        var made2 = itemComp.createObject(content)
        console.log("created with parent content: parent is content", made2.parent === content, content.children.length)

        second.parent = flick
        console.log("second.parent = flick: parent is flick", second.parent === flick, "is content", second.parent === content, flick.children.length, content.children.length)
        second.parent = content
        console.log("second.parent = content:", second.parent === content, flick.children.length, content.children.length)

        console.log("grid children", grid.children.length, grid.children[0] === grid.contentItem)
        console.log("grid content", grid.contentItem.children.length >= 3, grid.contentItem.parent === grid)
        Qt.quit()
    }
}
