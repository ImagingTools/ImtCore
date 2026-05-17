import QtQuick

Item {
    Item {
        id: mover
        x: 10
        y: 15
        width: 20
        height: 20
    }

    Item {
        id: target
        x: 100
        y: 120
        width: 50
        height: 60
    }

    Component.onCompleted: {
        console.log("start", mover.x, mover.y, mover.width, mover.height)

        mover.x += 5
        mover.y = target.y - 20
        mover.width = mover.width + 10
        mover.height = mover.height * 2

        console.log("step1", mover.x, mover.y, mover.width, mover.height)

        mover.anchors.left = target.right
        mover.anchors.top = target.bottom
        mover.anchors.leftMargin = 3
        mover.anchors.topMargin = 4

        console.log("step2", mover.x, mover.y, mover.width, mover.height)
        Qt.quit()
    }
}
