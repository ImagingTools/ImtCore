import QtQuick
Item {
    id: root
    property ListModel pages: ListModel {}
    property alias count: root.pages.count
    property int copied: root.count
    onCountChanged: console.log("count", count, copied)
    Component.onCompleted: {
        pages.append({name: "one"})
        console.log("append",count,copied)
        pages.clear()
        console.log("clear",count,copied)
        Qt.quit()
    }
}
