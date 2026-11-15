import QtQuick

// After scrolling a variable-height vertical ListView with reuseItems, rows do not stack and
// the reused delegate exposes the height of its new index.
Item {
    id: root
    width: 120
    height: 100

    ListView {
        id: view
        width: 120
        height: 100
        spacing: 2
        clip: true
        reuseItems: true
        model: 8
        delegate: Rectangle {
            width: view.width
            height: 20 + index * 10
            property int row: index
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
            view.contentY = 55
            view.forceLayout()
            var mid = view.itemAtIndex(2)
            console.log("mid-row", mid ? mid.row : -1)
            console.log("mid-height", mid ? mid.height : -1)
            console.log("mid-stacked", visibleStacked())

            view.contentY = 0
            view.forceLayout()
            var top = view.itemAtIndex(0)
            console.log("top-row", top ? top.row : -1)
            console.log("top-height", top ? top.height : -1)
            console.log("top-stacked", visibleStacked())
            Qt.quit()
        }
    }
}
