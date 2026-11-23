import QtQuick
Item { width: 400; height: 300
    ListModel { id: m; ListElement { h: 30; n: "a" } ListElement { h: 40; n: "b" } ListElement { h: 50; n: "c" } ListElement { h: 25; n: "d" } }
    ListView { id: list; width: 100; height: 100; model: m; delegate: Rectangle { width: 100; height: h; objectName: n } }
    function show(t) { list.forceLayout(); let out = []; for (let i = 0; i < m.count; i++) { let it = list.itemAtIndex(i); out.push(it ? it.objectName + "@" + it.y : "-") } console.log(t, out.join(" ")) }
    Component.onCompleted: { show("start"); m.move(0, 2, 1); show("move"); m.remove(1); show("remove"); Qt.quit() }
}
