import QtQuick 2.15
Item {
    id: root
    width: 300; height: 200
    property int made: 0
    property int step: 0
    function report(what) {
        var cells = []
        for (var i = 0; i < Math.min(9, grid.count); i++) {
            var item = grid.itemAtIndex(i)
            cells.push(i + ":" + (item ? item.uid + "@" + Math.round(item.x) + "," + Math.round(item.y) : "-"))
        }
        console.log(what, "contentY", Math.round(grid.contentY), "count", grid.count, "current", grid.currentIndex, "currentUid", grid.currentItem ? grid.currentItem.uid : -1, cells.join(" "))
    }
    ListModel { id: cells_ }
    GridView {
        id: grid
        anchors.fill: parent
        cellWidth: 100; cellHeight: 100
        model: cells_
        delegate: Item {
            property int uid: 0
            width: 100; height: 100
            Component.onCompleted: uid = ++root.made
        }
    }
    Timer {
        interval: 50; repeat: true; running: true
        onTriggered: {
            root.step++
            if (root.step === 1) { for (var i = 0; i < 12; i++) cells_.append({ n: i }) }
            else if (root.step === 2) { root.report("filled"); grid.currentIndex = 4 }
            else if (root.step === 3) { root.report("current 4"); cells_.move(1, 6, 2) }
            else if (root.step === 4) { root.report("moved 1,2 to 6"); cells_.insert(0, { n: -1 }) }
            else if (root.step === 5) { root.report("inserted at 0"); cells_.remove(2, 2) }
            else if (root.step === 6) { root.report("removed 2,3"); grid.currentIndex = 10 }
            else if (root.step >= 7 && root.step < 12) { if (root.step === 11) root.report("current 10 after 200 ms") }
            else { Qt.quit() }
        }
    }
}
