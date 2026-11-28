import QtQuick
Item {
    id: root
    property ListModel first: ListModel {}
    property ListModel second: ListModel {}
    property ListModel current: root.first
    property alias count: root.current.count
    property int mirrored: root.count
    onCountChanged: console.log("notify", count)
    Component.onCompleted: {
        first.append({name: "one"})
        console.log("first", count, mirrored)
        current = second
        console.log("switch", count, mirrored)
        first.append({name: "old"})
        second.append({name: "new"})
        console.log("done", count, mirrored)
        Qt.quit()
    }
}
