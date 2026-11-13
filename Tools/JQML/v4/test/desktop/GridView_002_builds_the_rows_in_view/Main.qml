import QtQuick

Item {
    id: root
    width: 300
    height: 300

    GridView {
        id: view
        width: 200
        height: 300
        cellWidth: 100
        cellHeight: 50
        cacheBuffer: 0
        model: 500
        delegate: Rectangle {
            width: 100
            height: 50
        }
    }

    // Every cell that touches the range of the content has a delegate.
    function covers(from, to) {
        for (var i = 0; i < 500; i++) {
            var top = Math.floor(i / 2) * 50
            if (top + 50 >= from && top <= to && view.itemAtIndex(i) === null) return false
        }
        return true
    }

    // No cell that is farther than `margin` from the range has one.
    function nothingFar(from, to, margin) {
        for (var i = 0; i < 500; i++) {
            var top = Math.floor(i / 2) * 50
            if ((top + 50 < from - margin || top > to + margin) && view.itemAtIndex(i) !== null) return false
        }
        return true
    }

    function count() {
        var n = 0
        for (var i = 0; i < 500; i++) {
            if (view.itemAtIndex(i) !== null) n++
        }
        return n
    }

    property int step: 0

    Timer {
        interval: 250
        running: true
        repeat: true
        onTriggered: {
            var y = view.contentY
            if (root.step === 0) {
                console.log("top", y, root.covers(y, y + 300), root.nothingFar(y, y + 300, 100), root.count() < 24)
                view.contentY = 5000
            } else if (root.step === 1) {
                console.log("on a row edge", y, root.covers(y, y + 300), root.nothingFar(y, y + 300, 100), root.count() < 24)
                view.contentY = 5025
            } else if (root.step === 2) {
                console.log("inside a row", y, root.covers(y, y + 300), root.nothingFar(y, y + 300, 100), root.count() < 24)
                view.cacheBuffer = 120
            } else if (root.step === 3) {
                console.log("buffer 120", y, root.covers(y, y + 300), root.nothingFar(y, y + 300, 220), root.count() < 40)
                view.contentY = 12200
            } else if (root.step === 4) {
                console.log("the end", y, root.covers(y, y + 300), root.nothingFar(y, y + 300, 220), root.count() < 40)
                Qt.quit()
            }
            root.step++
        }
    }
}
