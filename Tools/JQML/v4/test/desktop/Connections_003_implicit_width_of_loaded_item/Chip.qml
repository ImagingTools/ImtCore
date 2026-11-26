import QtQuick

// A chip: its implicitWidth follows the label, as the filter chips of ImtCore do (`implicitWidth: chip.width`).
Item {
    id: root
    property string value: ""
    implicitWidth: chip.width
    implicitHeight: 20

    Rectangle {
        id: chip
        width: label.width + 10
        height: 20
        Text {
            id: label
            text: "Status" + (root.value === "" ? "" : ": " + root.value)
        }
    }
}
