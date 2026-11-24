import QtQuick

// QQuickRepeater::initItem: the delegates are stacked before the Repeater, in the order of the model (insert, append, move)
Item {
    ListModel { id: m; ListElement { n: "a" } ListElement { n: "b" } ListElement { n: "c" } }
    Row { id: row; Item { objectName: "first" } Repeater { id: rep; model: m; Item { objectName: n } } Item { objectName: "last" } }
    function order() {
        let s = []
        for (let i = 0; i < row.children.length; i++) s.push(row.children[i] === rep ? "R" : row.children[i].objectName)
        return s.join(",")
    }
    Component.onCompleted: {
        console.log(order())
        m.append({ n: "d" }); console.log("append", order())
        m.insert(0, { n: "z" }); console.log("insert 0", order())
        m.insert(2, { n: "y" }); console.log("insert 2", order())
        m.move(0, 3, 2); console.log("move", order())
        m.remove(1); console.log("remove", order())
        m.clear(); console.log("clear", order())
        m.append({ n: "q" }); console.log("again", order())
        Qt.quit()
    }
}
