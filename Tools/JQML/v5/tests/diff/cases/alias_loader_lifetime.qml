import QtQuick

Item {
    id: root
    property int changes: 0
    property int destructions: 0

    Loader {
        id: loader
        sourceComponent: Component {
            Item {
                id: content
                property ListModel model: ListModel {}
                property alias count: content.model.count
                property int boundCount: content.count
                onCountChanged: root.changes++
                Component.onDestruction: root.destructions++
            }
        }
    }

    Component.onCompleted: {
        loader.item.model.append({ name: "before" })
        console.log("count", loader.item.boundCount, changes)
        loader.active = false
        loader.active = true
        loader.item.model.append({ name: "after" })
        console.log("new", loader.item.boundCount, changes)
        report.restart()
    }
    Timer {
        id: report
        interval: 50
        onTriggered: {
            console.log("destroyed", root.destructions)
            Qt.quit()
        }
    }
}
