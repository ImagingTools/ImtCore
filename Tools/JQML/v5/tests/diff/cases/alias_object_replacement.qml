import QtQuick
Item {
    id: root
    property ListModel first: ListModel {}
    property ListModel second: ListModel {}
    property ListModel current: ListModel {}
    property alias count: root.current.count
    property int mirrored: root.count
    onCountChanged: console.log("notify", count)
    Component.onCompleted: {
        let original = current
        current.append({name: "one"})
        console.log("first", count, mirrored)
        current = second
        console.log("switch", count, mirrored)
        original.append({name: "old"})
        second.append({name: "new"})
        console.log("done", count, mirrored)
        Qt.quit()
    }
}
