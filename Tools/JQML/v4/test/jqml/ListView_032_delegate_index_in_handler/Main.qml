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

    ListView {
        id: view
        anchors.fill: parent
        model: rows
        reuseItems: true
        delegate: Item {
            id: delegateItem
            width: view.width
            height: 30

            // what a click handler of a tree does: add rows below this one, change the view, use the index again
            function open() {
                var first = index
                rows.insert(index + 1, { name: "child 1" })
                rows.insert(index + 2, { name: "child 2" })
                view.currentIndex = 2
                return first + " " + index + " " + model.name
            }
        }
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            console.log("handler", view.itemAtIndex(0).open())
            Qt.quit()
        }
    }
}
