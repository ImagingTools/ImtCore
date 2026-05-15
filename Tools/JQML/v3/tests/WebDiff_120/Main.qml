import QtQuick

Item {
    ListModel { id: m }
    ListView {
        id: v
        width: 110
        height: 50
        model: m
        delegate: Rectangle { width: 22; height: 11 }
    }

    Component.onCompleted: {
        m.append({ a: 1 })
        m.append({ a: 2 })
        m.append({ a: 3 })
        m.clear()
        m.append({ a: 120 })
        console.log("wd120", "listmodel-clear-readd", m.count, v.count)
        Qt.quit()
    }
}
