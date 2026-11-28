import QtQuick

Item {
    id: root
    property int horizontal: 5
    property int vertical: 6
    property bool overrideValue: false
    property string field: "fieldX"

    Box {
        id: box
        fieldX: root.horizontal
        fieldY: root.vertical
    }

    Binding {
        target: box
        property: root.field
        value: 21
        when: root.overrideValue
    }

    Component.onCompleted: {
        root.overrideValue = true
        root.horizontal = 7
        root.vertical = 8
        console.log("x", box.fieldX, box.fieldY)
        root.field = "fieldY"
        root.horizontal = 9
        root.vertical = 10
        console.log("y", box.fieldX, box.fieldY)
        root.overrideValue = false
        console.log("restore", box.fieldX, box.fieldY)
        Qt.quit()
    }
}
