import QtQuick

Item {
    id: root
    objectName: "root"

    Component {
        id: cellComponent
        Item {
            property int columnIndex: -1
            property string label: "none"
            property bool active: false
            property real size: 2
            property int extra: 5
            property int fromBinding: extra + 1
            Component.onCompleted: {
                console.log("completed index=" + columnIndex + " label=" + label + " active=" + active + " size=" + size + " extra=" + extra + " fromBinding=" + fromBinding)
            }
        }
    }

    Component.onCompleted: {
        var full = cellComponent.createObject(root, { columnIndex: 3, label: "full", active: true, size: 8, fromBinding: 40 })
        console.log("after index=" + full.columnIndex + " label=" + full.label + " active=" + full.active + " size=" + full.size + " fromBinding=" + full.fromBinding)

        var partial = cellComponent.createObject(root, { extra: 8, label: "partial" })
        console.log("partial label=" + partial.label + " extra=" + partial.extra + " fromBinding=" + partial.fromBinding + " index=" + partial.columnIndex + " active=" + partial.active)
        Qt.quit()
    }
}
