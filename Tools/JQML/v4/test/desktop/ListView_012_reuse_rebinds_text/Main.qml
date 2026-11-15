import QtQuick

// reuseItems must refresh model-role bindings when a pooled delegate is shown for another row.
Item {
    id: root
    width: 120
    height: 80

    ListModel {
        id: rows
        ListElement { title: "A" }
        ListElement { title: "B" }
        ListElement { title: "C" }
        ListElement { title: "D" }
        ListElement { title: "E" }
        ListElement { title: "F" }
    }

    ListView {
        id: view
        anchors.fill: parent
        model: rows
        reuseItems: true
        clip: true
        spacing: 0

        delegate: Item {
            width: view.width
            height: 40
            property string label: model.title
            Text {
                anchors.fill: parent
                text: parent.label
            }
        }
    }

    Timer {
        interval: 1
        running: true
        onTriggered: {
            // Scroll far enough that index 0 is pooled, then return so it is reused for row 0.
            view.contentY = 160
            view.forceLayout()
            view.contentY = 0
            view.forceLayout()
            var top = view.itemAtIndex(0)
            console.log("reuse", top ? top.label : "none")
            Qt.quit()
        }
    }
}
