import QtQuick

// ControlBase-style width loop: auto-sized control follows content; explicit width is pushed to the decorator.
Item {
    id: root
    width: 400
    height: 80

    Item {
        id: autoSized
        width: decorator_ ? decorator_.width : 0
        height: decorator_ ? decorator_.height : 0
        property Component decorator: boxA
        property var decorator_: null
        onDecoratorChanged: {
            if (decorator_) { decorator_.destroy(); decorator_ = null }
            if (!decorator) return
            decorator_ = decorator.createObject(autoSized)
            bindWA.target = decorator_
            bindHA.target = decorator_
        }
        Binding { id: bindWA; property: "width"; value: autoSized.width }
        Binding { id: bindHA; property: "height"; value: autoSized.height }
    }

    Item {
        id: fixed
        x: 200
        width: 120
        height: decorator_ ? decorator_.height : 0
        property Component decorator: boxB
        property var decorator_: null
        onDecoratorChanged: {
            if (decorator_) { decorator_.destroy(); decorator_ = null }
            if (!decorator) return
            decorator_ = decorator.createObject(fixed)
            bindWB.target = decorator_
            bindHB.target = decorator_
        }
        Binding { id: bindWB; property: "width"; value: fixed.width }
        Binding { id: bindHB; property: "height"; value: fixed.height }
    }

    Component {
        id: boxA
        Item {
            width: 100
            height: 32
        }
    }

    Component {
        id: boxB
        Item {
            width: 60
            height: 32
        }
    }

    Component.onCompleted: {
        console.log("auto", autoSized.width)
        console.log("autoDec", autoSized.decorator_ ? autoSized.decorator_.width : -1)
        console.log("fixed", fixed.width)
        console.log("fixedDec", fixed.decorator_ ? fixed.decorator_.width : -1)
        Qt.quit()
    }
}
