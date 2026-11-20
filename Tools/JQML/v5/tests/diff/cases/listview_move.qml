import QtQuick 2.15
Item {
    id: root
    width: 300; height: 200
    property int made: 0
    property int step: 0
    function report(what) {
        var rows = []
        for (var i = 0; i < rows_.count; i++) {
            var item = list.itemAtIndex(i)
            if (item && item.y + item.height > list.contentY && item.y < list.contentY + list.height)
                rows.push(i + ":" + item.uid + "@" + Math.round(item.y))
        }
        console.log(what, "contentY", Math.round(list.contentY), "current", list.currentIndex, "currentUid", list.currentItem ? list.currentItem.uid : -1, "rows", rows.join(" "))
    }
    ListModel { id: rows_ }
    ListView {
        id: list
        anchors.fill: parent
        model: rows_
        cacheBuffer: 0
        delegate: Item {
            property int uid: 0
            width: 300; height: 20
            Component.onCompleted: uid = ++root.made
        }
    }
    Timer {
        interval: 50; repeat: true; running: true
        onTriggered: {
            root.step++
            if (root.step === 1) { for (var i = 0; i < 60; i++) rows_.append({ n: i }) }
            else if (root.step === 2) { root.report("filled"); list.currentIndex = 3 }
            else if (root.step === 3) { root.report("current 3"); rows_.move(2, 5, 1) }
            else if (root.step === 4) { root.report("moved 2 to 5"); rows_.move(3, 0, 2) }
            else if (root.step === 5) { root.report("moved 3,4 to 0"); rows_.move(1, 40, 1) }
            else if (root.step === 6) { root.report("moved 1 out to 40"); rows_.move(50, 2, 3) }
            else if (root.step === 7) { root.report("moved 50..52 in to 2"); list.contentY = 400 }
            else if (root.step === 8) { root.report("scrolled"); rows_.move(25, 21, 2) }
            else { root.report("moved 25,26 to 21"); Qt.quit() }
        }
    }
}
