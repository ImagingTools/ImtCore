import QtQuick

Container2 {
    id: root
    width: 200
    height: 200
    default property alias contentColumn: innerColumn.data

    Column {
        id: innerColumn
        spacing: 5
        objectName: "innerColumn"
    }

    Rectangle {
        width: 50
        height: 50
        color: "blue"

        onParentChanged: console.log("blue.onParentChanged", parent.objectName)
    }

    Rectangle {
        width: 50
        height: 50
        color: "green"
        onParentChanged: console.log("green.onParentChanged", parent.objectName)
    }

    Rectangle {
        width: 50
        height: 50
        color: "red"
        onParentChanged: console.log("red.onParentChanged", parent.objectName)
    }
}
