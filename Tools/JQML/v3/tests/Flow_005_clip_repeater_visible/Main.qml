import QtQuick

Item {
    id: root
    width: 400
    height: 200

    property var items: []

    Column {
        id: column
        width: parent.width

        Flow {
            id: flow
            width: parent.width
            clip: true
            spacing: 4
            visible: root.items.length > 0

            Repeater {
                id: rep
                model: root.items
                delegate: Rectangle {
                    width: 80
                    height: 28
                    property string itemId: modelData.id
                }
            }
        }
    }

    Timer {
        id: assignTimer
        interval: 20
        repeat: false
        onTriggered: {
            root.items = [{ "id": "a" }, { "id": "b" }]
            doneTimer.start()
        }
    }

    Timer {
        id: doneTimer
        interval: 20
        repeat: false
        onTriggered: {
            flow.forceLayout()
            var item0 = rep.itemAt(0)
            console.log("count", root.items.length, rep.count)
            console.log("child", item0 ? item0.width : 0, item0 ? item0.height : 0, item0 ? item0.visible : false)
            console.log("flowh", Math.round(flow.height), Math.round(flow.implicitHeight))
            Qt.quit()
        }
    }

    Component.onCompleted: {
        assignTimer.start()
    }
}
