import QtQuick

Item {
    width: 200
    height: 300

    ListModel {
        id: rows
        ListElement { name: "a" }
        ListElement { name: "b" }
        ListElement { name: "c" }
    }

    QtObject {
        id: holder
        property var item: null
    }

    ListView {
        id: view
        anchors.fill: parent
        model: rows
        reuseItems: true
        delegate: Item {
            property string rowName: model.name
            property int rowIndex: index
            width: view.width
            height: 30
        }
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            var b = view.itemAtIndex(1)
            holder.item = b
            console.log("before", b.rowName, b.rowIndex)
            rows.insert(0, { name: "x" })
            rows.insert(0, { name: "y" })
            step.start()
        }
    }

    Timer {
        id: step
        interval: 50
        onTriggered: {
            var b = view.itemAtIndex(3)
            console.log("after insert", b.rowName, b.rowIndex, view.itemAtIndex(0).rowName, view.count, b === holder.item)
            rows.remove(0)
            last.start()
        }
    }

    Timer {
        id: last
        interval: 50
        onTriggered: {
            var b = view.itemAtIndex(2)
            console.log("after remove", b.rowName, b.rowIndex, view.count)
            Qt.quit()
        }
    }
}
