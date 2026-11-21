import QtQuick

// PathView (QQuickPathView): items along the path by offset, pathItemCount, the highlight range, the current index,
// attributes of the path, positionViewAtIndex, the model changes
Item {
    width: 400; height: 300
    ListModel { id: lm; ListElement { name: "a" } ListElement { name: "b" } ListElement { name: "c" } ListElement { name: "d" } ListElement { name: "e" } ListElement { name: "f" } }
    PathView {
        id: pv
        width: 400; height: 300
        model: lm
        pathItemCount: 4
        preferredHighlightBegin: 0.5
        preferredHighlightEnd: 0.5
        highlightMoveDuration: 0
        delegate: Rectangle {
            width: 20; height: 10
            property string label: name
            property real sc: PathView.itemScale === undefined ? -1 : PathView.itemScale
            property bool cur: PathView.isCurrentItem
            property bool on: PathView.onPath
        }
        path: Path {
            startX: 0; startY: 100
            PathAttribute { name: "itemScale"; value: 0.5 }
            PathLine { x: 200; y: 100 }
            PathAttribute { name: "itemScale"; value: 1.0 }
            PathLine { x: 400; y: 100 }
            PathAttribute { name: "itemScale"; value: 0.5 }
        }
    }
    PathView {
        id: free
        y: 200; width: 400; height: 100
        model: 5
        highlightRangeMode: PathView.NoHighlightRange
        delegate: Item { width: 10; height: 10 }
        path: Path { startX: 0; startY: 0; PathLine { x: 300; y: 0 } }
    }
    function dump(view) {
        let out = []
        for (let i = 0; i < view.count; i++) {
            let it = view.itemAtIndex(i)
            out.push(it ? i + ":" + Math.round(it.x) + (it.sc !== undefined ? "/" + it.sc.toFixed(2) : "") + (it.cur ? "*" : "") + (it.on === false ? "-" : "") : i + ":none")
        }
        return out.join(" ")
    }
    Component.onCompleted: {
        console.log(pv.count, pv.currentIndex, pv.offset, dump(pv))
        pv.currentIndex = 2
        console.log(pv.currentIndex, pv.offset, dump(pv))
        pv.incrementCurrentIndex()
        console.log(pv.currentIndex, pv.offset, dump(pv))
        pv.offset = 1.5
        console.log(pv.currentIndex, pv.offset, dump(pv))
        pv.positionViewAtIndex(4, PathView.Beginning)
        console.log(pv.currentIndex, pv.offset)
        lm.remove(0)
        console.log(pv.count, pv.currentIndex, pv.offset)
        lm.append({ name: "g" })
        console.log(pv.count, pv.currentIndex, pv.offset)
        console.log(free.count, free.currentIndex, free.offset, dump(free))
        free.offset = 1
        console.log(free.currentIndex, dump(free))
        free.currentIndex = 3
        console.log(free.currentIndex, free.offset, PathView.StrictlyEnforceRange, PathView.SnapOneItem, PathView.Contain)
        Qt.quit()
    }
}
