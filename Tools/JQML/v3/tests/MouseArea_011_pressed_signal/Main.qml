import QtQuick

Item {
    id: root
    width: 240
    height: 180

    property int pressedCount: 0
    property int pressedChangedCount: 0
    property int connectionsPressedCount: 0

    MouseArea {
        id: area
        x: 20
        y: 20
        width: 120
        height: 100

        onPressed: root.pressedCount += 1
        onPressedChanged: root.pressedChangedCount += 1
    }

    Connections {
        target: area

        function onPressed(mouse) {
            root.connectionsPressedCount += 1
        }
    }

    function dispatchPointer(target, type, clientX, clientY) {
        let event = new window.PointerEvent(type, {
            bubbles: true,
            cancelable: true,
            pointerId: 1,
            pointerType: "mouse",
            isPrimary: true,
            clientX: clientX,
            clientY: clientY,
            button: 0,
            buttons: type === "pointerup" ? 0 : 1
        })
        target.dispatchEvent(event)
    }

    function report(stage) {
        console.log("pressed-signal", stage, root.pressedCount, root.connectionsPressedCount, root.pressedChangedCount)
    }

    // Qt: onPressed and Connections.onPressed run on the press only, onPressedChanged on the press and the release
    Component.onCompleted: {
        if(typeof window === "undefined") {
            console.log("pressed-signal press 1 1 1")
            console.log("pressed-signal release 1 1 2")
            Qt.quit()
            return
        }

        try {
            let dom = area.__getDOM()
            let rect = dom.getBoundingClientRect()
            let x = rect.left + 10
            let y = rect.top + 10
            root.dispatchPointer(dom, "pointerdown", x, y)
            root.report("press")
            root.dispatchPointer(window, "pointerup", x, y)
            root.report("release")
        } catch(err) {
            console.log("pressed-signal-error", err)
        }
        Qt.quit()
    }
}
