import QtQuick 2.15
Item {
    id: root
    width: 300; height: 400
    property int step: 0
    function report(what) {
        var top = list.indexAt(10, list.contentY + 1)
        var item = list.itemAtIndex(top)
        console.log(what, "contentY", Math.round(list.contentY), "originY", Math.round(list.originY), "contentHeight", Math.round(list.contentHeight),
                    "top", top, "topY", item ? Math.round(item.y) : "none", "count", list.count, "current", list.currentIndex)
    }
    ListModel { id: rows }
    ListView {
        id: list
        anchors.fill: parent
        model: rows
        delegate: Item { width: 300; height: 30 }
    }
    Timer {
        interval: 50; repeat: true; running: true
        onTriggered: {
            root.step++
            if (root.step === 1) { for (var i = 0; i < 200; i++) rows.append({ n: i }); }
            else if (root.step === 2) { root.report("filled"); list.contentY = 1500 }
            else if (root.step === 3) { root.report("scrolled"); rows.insert(10, { n: -1 }); rows.insert(10, { n: -2 }) }
            else if (root.step === 4) { root.report("inserted above"); rows.remove(0, 5) }
            else if (root.step === 5) { root.report("removed above"); rows.insert(52, { n: -3 }) }
            else if (root.step === 6) { root.report("inserted in view"); rows.remove(48, 6) }
            else if (root.step === 7) { root.report("removed in view"); list.currentIndex = 150 }
            else { root.report("current 150"); Qt.quit() }
        }
    }
}
