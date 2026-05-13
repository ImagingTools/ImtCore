import QtQuick

Item {
    Flow {
        id: flow
        spacing: 1

        Item { id: a; width: 25; height: 10 }
        Item { id: b; width: 30; height: 20 }
    }

    Timer {
        id: t
        interval: 1
        repeat: false
        onTriggered: {
            console.log("after", "updated")
            Qt.quit()
        }
    }

    Component.onCompleted: {
        console.log("before", "created")
        flow.spacing = 4
        b.visible = false
        flow.forceLayout()
        t.start()
    }
}
