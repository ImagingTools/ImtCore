import QtQuick

// ControlBase-style loop: control height follows the decorator; Binding pushes that height back.
// `decorator` is bound (as with Style.buttonDecorator) so onDecoratorChanged runs inside a binding
// sweep — Binding must not stamp height 0 before control.height catches up.
Item {
    id: control
    width: decorator_ ? decorator_.width : 0
    height: decorator_ ? decorator_.height : 0

    property Component decorator: buttonDecorator
    property var decorator_: null

    onDecoratorChanged: {
        if (decorator_) {
            decorator_.destroy()
            decorator_ = null
        }
        if (decorator == null || decorator == undefined)
            return
        decorator_ = decorator.createObject(control)
        bindHeight.target = decorator_
    }

    Binding {
        id: bindHeight
        property: "height"
        value: control.height
    }

    Component {
        id: buttonDecorator
        Item {
            width: 80
            height: 32
        }
    }

    Component.onCompleted: {
        console.log("height", control.height)
        console.log("decHeight", decorator_ ? decorator_.height : -1)
        Qt.quit()
    }
}
