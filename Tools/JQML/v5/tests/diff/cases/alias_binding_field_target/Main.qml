import QtQuick

Item {
    id: root

    property int horizontal: 5
    property int vertical: 6
    property bool overrideValue: false

    Box {
        id: box
        fieldX: root.horizontal
        fieldY: root.vertical
    }

    Binding {
        target: box
        property: "fieldX"
        value: 21
        when: root.overrideValue
    }

    Component.onCompleted: {
        console.log("initial", box.fieldX, box.fieldY)
        root.overrideValue = true
        root.horizontal = 7
        root.vertical = 8
        console.log("active", box.fieldX, box.fieldY)
        root.overrideValue = false
        console.log("restored", box.fieldX, box.fieldY)
        Qt.quit()
    }
}
