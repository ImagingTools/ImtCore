import QtQuick

// The pattern of ControlBase and DialogDecorator: a control creates its decorator with createObject and then points a Binding at it
// (the Binding carries the width of the control to the decorator). The column of the decorator writes the width of the decorator by a
// script when its own width is known (it is known while the decorator is created, before the Binding has its target). The Binding gives
// its value to the target when it gets it, so the width of the control wins.
Item {
    id: root
    width: 400
    height: 300

    Component {
        id: decoration
        Item {
            id: deco
            property var baseElement: null
            width: column ? column.width : 0
            height: 20
            Column {
                id: column
                width: Math.max(buttonsWidth, bodyWidth)
                property real bodyWidth: 1
                property real buttonsWidth: 32
                onWidthChanged: deco.width = width
            }
        }
    }

    FocusScope {
        id: control
        width: 300
        height: 100
        property Component decorator: decoration
        property var decorator_: null
        onDecoratorChanged: {
            decorator_ = decorator.createObject(control)
            decorator_.baseElement = control
            bindWidth.target = decorator_
        }
        Binding {
            id: bindWidth
            property: "width"
            value: control.width
        }
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            console.log("decorator width", control.decorator_.width, "control width", control.width)
            Qt.quit()
        }
    }
}
