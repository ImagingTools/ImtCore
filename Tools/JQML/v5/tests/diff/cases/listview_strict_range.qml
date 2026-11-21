import QtQuick
import QtTest

// ListView with StrictlyEnforceRange: where the view is made (contentY 0, the item at the highlight current), a range set
// after the model, a new model, a new current index
Item {
    width: 400; height: 400
    Component { id: d; Rectangle { width: 60; height: 40 } }
    ListView { id: a; width: 60; height: 200; model: 3; delegate: d; highlightRangeMode: ListView.StrictlyEnforceRange; preferredHighlightBegin: 80; preferredHighlightEnd: 120; snapMode: ListView.SnapToItem; highlightMoveDuration: 1000 }
    ListView { id: b; x: 100; width: 60; height: 200; highlightRangeMode: ListView.StrictlyEnforceRange; snapMode: ListView.SnapToItem; highlightMoveDuration: 1000 }
    ListView { id: c; x: 200; width: 60; height: 200; highlightRangeMode: ListView.StrictlyEnforceRange; snapMode: ListView.SnapToItem; highlightMoveDuration: 1000 }
    ListView { id: e; x: 300; width: 60; height: 200; model: 10; currentIndex: 5; delegate: d; highlightRangeMode: ListView.StrictlyEnforceRange; preferredHighlightBegin: 80; preferredHighlightEnd: 120 }
    ListView { id: f; x: 300; y: 200; width: 60; height: 200; model: 10; delegate: d; highlightRangeMode: ListView.ApplyRange; preferredHighlightBegin: 80; preferredHighlightEnd: 120 }
    TestCase { name: "L"; when: windowShown
        function test_a() {
            console.log("a", a.contentY, a.currentIndex, a.originY, "e", e.contentY, e.currentIndex, "f", f.contentY, f.currentIndex)
            b.preferredHighlightBegin = 80; b.preferredHighlightEnd = 120; b.model = 3; b.delegate = d
            c.model = 3; c.delegate = d; c.preferredHighlightBegin = 80; c.preferredHighlightEnd = 120
            wait(1500)
            console.log("a", a.contentY, "b", b.contentY, b.currentIndex, "c", c.contentY, c.currentIndex)
            b.model = 4; wait(1500); console.log("b model", b.contentY, b.currentIndex)
            b.currentIndex = 2; wait(1500); console.log("b 2", b.contentY); b.model = 5; wait(1500); console.log("b model 5", b.contentY, b.currentIndex)
        }
    }
}
