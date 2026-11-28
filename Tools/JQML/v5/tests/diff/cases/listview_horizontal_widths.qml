import QtQuick 2.15
Item {
    id: root
    width: 400; height: 300
    property int step: 0
    function report(what) {
        var top = list.indexAt(list.contentX + 1, 10)
        var item = list.itemAtIndex(top)
        console.log(what, "contentX", Math.round(list.contentX), "originX", Math.round(list.originX), "contentWidth", Math.round(list.contentWidth),
                    "top", top, "topX", item ? Math.round(item.x) : "none", "count", list.count)
    }
    ListView {
        id: list
        anchors.fill: parent
        orientation: ListView.Horizontal
        spacing: 2
        model: 300
        delegate: Item { height: 300; width: 10 + (index * 37) % 41 }
    }
    Timer {
        interval: 150; repeat: true; running: true
        onTriggered: {
            root.step++
            if (root.step === 1) { root.report("start"); list.contentX = 5000 }
            else if (root.step === 2) { root.report("jumped"); list.positionViewAtIndex(250, ListView.Beginning) }
            else if (root.step === 3) { root.report("at 250"); list.positionViewAtIndex(120, ListView.Center) }
            else if (root.step === 4) { root.report("at 120 center"); list.positionViewAtBeginning() }
            else if (root.step === 5) { root.report("beginning"); list.positionViewAtEnd() }
            else { root.report("end"); Qt.quit() }
        }
    }
}
