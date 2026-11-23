import QtQuick
Item {
    property var list: ["name", 2]
    property list<int> ints: [1, 2]
    Component.onCompleted: {
        console.log(["a"], [1, [2, 3]], list, ints, "x", [], {a: 1})
        console.log(list)
        console.log("s" + list)
        Qt.quit()
    }
}
