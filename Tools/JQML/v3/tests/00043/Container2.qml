import QtQuick

Item {
    id: root
    width: 200
    height: 200
    default property alias contentRow: innerRow.data

    Row {
        id: innerRow
        spacing: 5
        objectName: "innerRow"
    }
    
}
