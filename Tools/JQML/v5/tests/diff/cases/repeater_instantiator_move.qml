import QtQuick 2.15
Item {
    id: root
    property int made: 0
    property var log: []
    ListModel { id: m }
    Column {
        id: column
        Repeater {
            id: rep
            model: m
            delegate: Item { property int uid: 0; width: 10; height: 10; Component.onCompleted: uid = ++root.made }
            onItemAdded: function(index, item) { root.log.push("added " + index) }
            onItemRemoved: function(index, item) { root.log.push("removed " + index) }
        }
    }
    Instantiator {
        id: inst
        model: m
        delegate: QtObject { property int uid: 0; Component.onCompleted: uid = ++root.made }
        onObjectAdded: function(index, object) { root.log.push("object added " + index) }
        onObjectRemoved: function(index, object) { root.log.push("object removed " + index) }
    }
    function uids() {
        var a = [], b = [], ys = []
        for (var i = 0; i < m.count; i++) { a.push(rep.itemAt(i).uid); b.push(inst.objectAt(i).uid); ys.push(rep.itemAt(i).y) }
        return "repeater " + a.join(",") + " y " + ys.join(",") + " instantiator " + b.join(",") + " object " + inst.object.uid
    }
    Component.onCompleted: {
        for (var i = 0; i < 5; i++) m.append({ n: i })
        root.log = []
        console.log("start", uids())
        m.move(0, 3, 2)
        console.log("moved 0,1 to 3", uids(), "signals", root.log.join(";"))
        root.log = []
        m.move(4, 0, 1)
        console.log("moved 4 to 0", uids(), "signals", root.log.join(";"))
        Qt.quit()
    }
}
