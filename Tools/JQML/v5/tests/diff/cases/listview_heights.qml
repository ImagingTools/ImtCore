import QtQuick 2.15
Item {
    id: root
    width: 300; height: 400
    property int step: 0
    function report(what) {
        var top = list.indexAt(10, list.contentY + 1)
        var item = list.itemAtIndex(top)
        console.log(what, "contentY", Math.round(list.contentY), "originY", Math.round(list.originY), "contentHeight", Math.round(list.contentHeight),
                    "top", top, "topY", item ? Math.round(item.y) : "none", "count", list.count)
    }
    ListView {
        id: list
        anchors.fill: parent
        spacing: 2
        model: 300
        delegate: Item { width: 300; height: 10 + (index * 37) % 41 }
    }
    Timer {
        interval: 50; repeat: true; running: true
        onTriggered: {
            root.step++
            if (root.step === 1) { root.report("start"); list.contentY = 5000 }
            else if (root.step === 2) { root.report("jumped"); list.positionViewAtIndex(250, ListView.Beginning) }
            else if (root.step === 3) { root.report("at 250"); list.positionViewAtIndex(120, ListView.Center) }
            else if (root.step === 4) { root.report("at 120 center"); list.positionViewAtBeginning() }
            else if (root.step === 5) { root.report("beginning"); list.positionViewAtEnd() }
            else { root.report("end"); Qt.quit() }
        }
    }
}
