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
            Component.onCompleted: {
                console.log("completed index=" + columnIndex + " label=" + label + " active=" + active + " size=" + size)
            }
        }
    }

    function fillCell(index) {
        var obj = cellComponent.createObject(root)
        obj.columnIndex = index
        obj.label = "filled"
        obj.active = true
        obj.size = 12
        console.log("filled index=" + obj.columnIndex + " label=" + obj.label + " active=" + obj.active + " size=" + obj.size)
        return obj
    }

    Component.onCompleted: {
        var obj = cellComponent.createObject(root, { columnIndex: 1, label: "seed" })
        console.log("before index=" + obj.columnIndex + " label=" + obj.label + " active=" + obj.active)
        obj.columnIndex = 6
        obj.label = "later"
        obj.active = true
        obj.size = 9
        console.log("later index=" + obj.columnIndex + " label=" + obj.label + " active=" + obj.active + " size=" + obj.size)

        fillCell(2)
        fillCell(3)
        Qt.quit()
    }
}
