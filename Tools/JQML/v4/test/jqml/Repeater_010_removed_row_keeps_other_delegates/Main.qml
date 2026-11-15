import QtQuick

Item {
    id: page
    width: 200
    height: 200

    property bool logging: true

    ListModel {
        id: rows
        ListElement { name: "a" }
        ListElement { name: "b" }
        ListElement { name: "c" }
        ListElement { name: "d" }
    }

    Column {
        Repeater {
            id: repeater
            model: rows
            delegate: Item {
                property string key: model.name
                property int row: index
                width: 50
                height: 10

                Item {
                    property int innerRow: index
                    id: inner
                }

                property int innerIndex: inner.innerRow

                Component.onCompleted: console.log("created", key, row)
                Component.onDestruction: {
                    if (page.logging)
                        console.log("destroyed", key)
                }
            }
        }
    }

    property var saved: []

    function dump(label) {
        var parts = []
        for (var i = 0; i < repeater.count; ++i) {
            var item = repeater.itemAt(i)
            parts.push(item.key + ":" + item.row + "/" + item.innerIndex)
        }
        console.log(label, parts.join(" "), repeater.count)
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            saved = [repeater.itemAt(0), repeater.itemAt(2), repeater.itemAt(3)]
            dump("start")
            rows.remove(1)
            step.start()
        }
    }

    Timer {
        id: step
        interval: 50
        onTriggered: {
            dump("removed")
            console.log("same", saved[0] === repeater.itemAt(0), saved[1] === repeater.itemAt(1), saved[2] === repeater.itemAt(2))
            rows.insert(0, { name: "x" })
            last.start()
        }
    }

    Timer {
        id: last
        interval: 50
        onTriggered: {
            dump("inserted")
            console.log("same", saved[0] === repeater.itemAt(1), saved[1] === repeater.itemAt(2), saved[2] === repeater.itemAt(3))
            logging = false
            Qt.quit()
        }
    }
}
