import QtQuick

Item {
    id: root
    width: 200
    height: 100

    Rectangle {
        id: box
        width: 100
        height: 50

        WheelHandler {
            id: wheel
            property: "x"
            rotationScale: 1 / 120
        }

        DragHandler {
            id: drag
        }
    }

    Rectangle {
        id: other
        width: 10
        height: 10

        WheelHandler { id: plainWheel }

        DragHandler {
            id: fixedDrag
            target: null
            dragThreshold: 3
            xAxis.enabled: false
            yAxis.minimum: -5
            yAxis.maximum: 25
        }
    }

    Component.onCompleted: {
        console.log("wheel", wheel.enabled, wheel.property, wheel.rotationScale, wheel.rotation)
        console.log("wheel target", wheel.target === box, wheel.parent === box)
        console.log("plain", plainWheel.property === "", plainWheel.rotationScale, plainWheel.orientation === Qt.Vertical)
        console.log("devices", wheel.acceptedDevices === (PointerDevice.Mouse | PointerDevice.TouchPad), PointerDevice.Mouse, PointerDevice.TouchPad, PointerDevice.AllDevices)
        console.log("drag", drag.enabled, drag.active, drag.translation.x, drag.translation.y)
        console.log("drag target", drag.target === box, drag.parent === box, fixedDrag.target === null)
        console.log("axes", drag.xAxis.enabled, drag.yAxis.enabled, drag.xAxis.minimum, drag.xAxis.maximum)
        console.log("fixed", fixedDrag.dragThreshold, fixedDrag.xAxis.enabled, fixedDrag.yAxis.minimum, fixedDrag.yAxis.maximum)
        console.log("threshold", drag.dragThreshold > 0)
        console.log("active translation", drag.activeTranslation.x, drag.persistentTranslation.y)
        console.log("buttons", drag.acceptedButtons === Qt.LeftButton, wheel.acceptedButtons === Qt.LeftButton)
        Qt.quit()
    }
}
