import QtQuick

// reuseItems scroll must not stack delegates on top of each other, and must rebind the top row.
Item {
    id: root
    width: 100
    height: 120

    ListModel {
        id: rows
        ListElement { title: "A" }
        ListElement { title: "B" }
        ListElement { title: "C" }
        ListElement { title: "D" }
        ListElement { title: "E" }
        ListElement { title: "F" }
        ListElement { title: "G" }
        ListElement { title: "H" }
        ListElement { title: "I" }
        ListElement { title: "J" }
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

    function visibleStacked() {
        var prevBottom = -1e9
        var count = 0
        for (var i = 0; i < view.count; i++) {
            var it = view.itemAtIndex(i)
            if (!it)
                continue
            count++
            if (it.y < prevBottom - 0.01)
                return "yes"
            prevBottom = it.y + it.height
        }
        return count > 0 ? "no" : "empty"
    }

    Timer {
        interval: 1
        running: true
        onTriggered: {
            view.contentY = 200
            view.forceLayout()
            var mid = view.itemAtIndex(5)
            console.log("mid-label", mid ? mid.label : "none")
            console.log("mid-stacked", visibleStacked())

            view.contentY = 0
            view.forceLayout()
            var top = view.itemAtIndex(0)
            console.log("top-label", top ? top.label : "none")
            console.log("top-stacked", visibleStacked())
            Qt.quit()
        }
    }
}
