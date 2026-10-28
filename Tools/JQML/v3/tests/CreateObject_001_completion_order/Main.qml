import QtQuick

Item {
    id: root
    objectName: "root"

    Component {
        id: cellComponent
        Item {
            property int columnIndex: -1
            property string label: "none"
            Component.onCompleted: {
                console.log("completed index=" + columnIndex + " label=" + label + " parent=" + (parent ? parent.objectName : "null") + " inner=" + children.length)
            }
            Item {
                width: 4
                height: 6
            }
        }
    }

    Component.onCompleted: {
        var plain = cellComponent.createObject(root)
        console.log("after index=" + plain.columnIndex + " label=" + plain.label + " parent=" + plain.parent.objectName)

        var empty = cellComponent.createObject(root, {})
        console.log("empty index=" + empty.columnIndex + " label=" + empty.label)

        var lone = cellComponent.createObject(null)
        console.log("lone parent=" + (lone.parent ? lone.parent.objectName : "null") + " index=" + lone.columnIndex)
        Qt.quit()
    }
}
