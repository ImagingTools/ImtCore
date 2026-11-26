import QtQuick
import QtQuick.Templates as T
import QtTest

// a Tumbler whose contentItem is a PathView of the document (TumblerThemed of QmlAppTemplate): the Tumbler follows its
// current index and count, sizes its delegates, Tumbler.displacement from its offset
Item {
    id: root
    width: 200; height: 300
    T.Tumbler {
        id: t
        width: 100; height: 250
        model: 10
        visibleItemCount: 5
        contentItem: PathView {
            id: pathView
            model: t.model
            delegate: t.delegate
            clip: true
            pathItemCount: t.visibleItemCount + 1
            preferredHighlightBegin: 0.5
            preferredHighlightEnd: 0.5
            dragMargin: width / 2
            path: Path {
                startX: pathView.width / 2
                startY: -pathView.delegateHeight / 2
                PathLine { x: pathView.width / 2; y: pathView.pathItemCount * pathView.delegateHeight - pathView.delegateHeight / 2 }
            }
            property real delegateHeight: t.availableHeight / t.visibleItemCount
        }
        delegate: Text {
            required property var modelData
            required property int index
            text: modelData
            property real d: T.Tumbler.displacement
        }
    }
    function dump() {
        let out = []
        for (let i = 0; i < t.count; i++) {
            let it = pathView.itemAtIndex(i)
            if (it) out.push(i + ":" + Math.round(it.y) + "/" + it.width + "x" + it.height + "/" + it.d.toFixed(2))
        }
        return out.join(" ")
    }
    TestCase {
        name: "TumblerPathView"
        when: windowShown
        function test_view() {
            console.log(t.count, t.currentIndex, pathView.currentIndex, t.currentItem === pathView.currentItem, dump())
            t.currentIndex = 3
            tryCompare(pathView, "offset", 7, 2000)
            console.log(t.currentIndex, pathView.currentIndex, pathView.offset, dump())
            pathView.currentIndex = 8
            tryCompare(pathView, "offset", 2, 2000)
            console.log(t.currentIndex, t.moving, dump())
        }
    }
}
