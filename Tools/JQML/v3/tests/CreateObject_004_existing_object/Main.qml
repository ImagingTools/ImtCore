import QtQuick

Item {
    id: root
    objectName: "root"

    Item {
        id: host
        objectName: "host"
        property int token: 3
        Item {
            objectName: "hostChild"
            width: 1
            height: 1
        }
    }

    Component {
        id: cellComponent
        Item {
            property int columnIndex: -1
            property Item rowDelegate: null
            Component.onCompleted: {
                console.log("completed index=" + columnIndex + " hasRow=" + (rowDelegate === host) + " rowName=" + (rowDelegate ? rowDelegate.objectName : "null"))
            }
        }
    }

    Component.onCompleted: {
        host.token = 9
        var childCount = host.children.length

        var mapped = cellComponent.createObject(root, { columnIndex: 2, rowDelegate: host })
        console.log("mapped same=" + (mapped.rowDelegate === host) + " index=" + mapped.columnIndex + " token=" + host.token + " children=" + host.children.length + " sameChildren=" + (host.children.length === childCount))

        var assigned = cellComponent.createObject(root)
        assigned.columnIndex = 3
        assigned.rowDelegate = host
        console.log("assigned same=" + (assigned.rowDelegate === host) + " index=" + assigned.columnIndex + " token=" + host.token + " children=" + host.children.length)
        Qt.quit()
    }
}
