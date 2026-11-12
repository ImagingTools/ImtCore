import QtQuick

Item {
    width: 300
    height: 80

    ListModel {
        id: rows
        ListElement { title: "a" }
        ListElement { title: "b" }
        ListElement { title: "c" }
        ListElement { title: "d" }
        ListElement { title: "e" }
        ListElement { title: "f" }
    }

    ListView {
        id: view
        anchors.fill: parent
        orientation: ListView.Horizontal
        model: rows

        // like the filter panel: the width of the delegate is made of items that are declared after it
        delegate: Item {
            id: cell
            width: Math.max(selected.width + selected.x, body.width)
            height: view.height

            Rectangle {
                id: selected
                x: 0
                width: 0
                height: 10
            }

            Rectangle {
                id: body
                width: Math.max(100, label.width)
                height: view.height

                Text {
                    id: label
                    text: model.title
                }
            }
        }
    }

    Timer {
        interval: 100
        running: true
        onTriggered: {
            console.log("count", view.count)
            console.log("contentWidth", view.contentWidth)
            Qt.quit()
        }
    }
}
