import QtQuick

// An anchor may point at the parent or at a sibling only. An item that is not (a child of a sibling, a grandparent, an item of another
// branch) is not a valid target: Qt reports it and the anchor does nothing, margin included. The position stays as it was.
Item {
    id: root
    width: 400
    height: 300

    Item {
        id: left
        width: 150
        height: 150
        Rectangle { id: title; x: 7; y: 5; width: 60; height: 15 }
    }

    Item {
        id: right
        x: 200
        width: 150
        height: 150

        Rectangle { id: ownTitle; x: 3; y: 4; width: 60; height: 15 }

        // child of a sibling: not valid
        Rectangle { id: cousin; width: 20; height: 10; anchors.top: title.bottom; anchors.topMargin: 30 }
        // sibling: valid
        Rectangle { id: sibling; width: 20; height: 10; anchors.top: ownTitle.bottom; anchors.topMargin: 30 }
        // grandparent: not valid
        Rectangle { id: grand; x: 40; width: 20; height: 10; anchors.top: root.bottom; anchors.topMargin: 3 }
        // parent: valid
        Rectangle { id: parented; x: 60; width: 20; height: 10; anchors.top: right.top; anchors.topMargin: 6 }
        // a column with a not valid anchor
        Column {
            id: column
            x: 90
            anchors.top: title.bottom
            anchors.topMargin: 20
            width: 30
            Rectangle { width: 30; height: 12 }
        }
        // the horizontal axis too
        Rectangle { id: horizontal; y: 100; width: 20; height: 10; anchors.left: title.right; anchors.leftMargin: 9 }
        Rectangle { id: centered; x: 5; y: 120; width: 20; height: 10; anchors.verticalCenter: title.verticalCenter }
    }

    Component.onCompleted: {
        console.log("cousin", cousin.x, cousin.y)
        console.log("sibling", sibling.x, sibling.y)
        console.log("grand", grand.x, grand.y)
        console.log("parented", parented.x, parented.y)
        console.log("column", column.x, column.y, column.height)
        console.log("horizontal", horizontal.x, horizontal.y)
        console.log("centered", centered.x, centered.y)
        Qt.quit()
    }
}
