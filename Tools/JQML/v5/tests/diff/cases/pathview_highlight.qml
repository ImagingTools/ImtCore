import QtQuick

// the highlight of a PathView: at the start of the range (StrictlyEnforceRange), else moved to the current item
Item {
    width: 400; height: 200
    PathView {
        id: strict
        width: 400; height: 100
        model: 6
        preferredHighlightBegin: 0.25
        preferredHighlightEnd: 0.25
        highlight: Rectangle { width: 30; height: 30; property bool on: PathView.onPath }
        delegate: Item { width: 10; height: 10 }
        path: Path { startX: 0; startY: 50; PathLine { x: 400; y: 50 } }
    }
    PathView {
        id: free
        y: 100; width: 400; height: 100
        model: 6
        highlightRangeMode: PathView.NoHighlightRange
        highlightMoveDuration: 0
        highlight: Rectangle { width: 30; height: 30 }
        delegate: Item { width: 10; height: 10 }
        path: Path { startX: 0; startY: 50; PathLine { x: 400; y: 50 } }
    }
    Timer {
        running: true; interval: 50
        onTriggered: {
            console.log(strict.highlightItem.x, strict.highlightItem.y, strict.highlightItem.on, strict.currentIndex, strict.offset)
            console.log(free.highlightItem.x, free.highlightItem.y, free.currentIndex)
            free.currentIndex = 2
            next.start()
        }
    }
    Timer {
        id: next; interval: 50
        onTriggered: {
            console.log(free.highlightItem.x, free.currentIndex, free.offset)
            strict.currentIndex = 1
            last.start()
        }
    }
    Timer {
        id: last; interval: 600
        onTriggered: {
            console.log(strict.highlightItem.x, strict.currentIndex, strict.offset, strict.itemAtIndex(1).x)
            Qt.quit()
        }
    }
}
