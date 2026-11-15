import QtQuick

Item {
    width: 200
    height: 300

    ListModel {
        id: rows
        ListElement { name: "a"; shown: true }
        ListElement { name: "b"; shown: true }
        ListElement { name: "c"; shown: true }
        ListElement { name: "d"; shown: true }
        ListElement { name: "e"; shown: true }
        ListElement { name: "f"; shown: true }
    }

    ListView {
        id: view
        anchors.fill: parent
        model: rows
        delegate: Item {
            width: view.width
            height: model.shown ? 30 : 0
        }
    }

    function positions() {
        var result = []
        for (var i = 0; i < rows.count; ++i) {
            var item = view.itemAtIndex(i)
            result.push(item ? item.y + "/" + item.height : "-")
        }
        return result.join(" ")
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            console.log("all shown", positions(), view.contentHeight)
            rows.setProperty(1, "shown", false)
            rows.setProperty(2, "shown", false)
            step.start()
        }
    }

    Timer {
        id: step
        interval: 50
        onTriggered: {
            console.log("two hidden", positions(), view.contentHeight)
            rows.setProperty(1, "shown", true)
            last.start()
        }
    }

    Timer {
        id: last
        interval: 50
        onTriggered: {
            console.log("one back", positions(), view.contentHeight)
            Qt.quit()
        }
    }
}
