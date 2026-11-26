import QtQuick
import QtTest

// What desktop Qt 6.11 does when a Flickable is dragged and released (QtTest sends the mouse events to the window).
// Run: QT_QPA_FONTDIR=C:/Windows/Fonts QT_QPA_PLATFORM=offscreen QT_FORCE_STDERR_LOGGING=1 qml -f Main.qml
Item {
    id: root
    width: 400
    height: 400
    property string log: ""

    TestCase { id: tc; name: "measure"; when: windowShown }

    Flickable {
        id: flick
        x: 0; y: 0; width: 300; height: 300
        contentWidth: 1500
        contentHeight: 300
        boundsBehavior: Flickable.StopAtBounds
        onMovementStarted: root.log += "MS(" + flick.contentX.toFixed(1) + ") "
        onMovementEnded: root.log += "ME(" + flick.contentX.toFixed(1) + ") "
        onFlickStarted: root.log += "FS(" + flick.contentX.toFixed(1) + ") "
        onFlickEnded: root.log += "FE(" + flick.contentX.toFixed(1) + ") "
        onDragStarted: root.log += "DS(" + flick.contentX.toFixed(1) + ") "
        onDragEnded: root.log += "DE(" + flick.contentX.toFixed(1) + ") "
        onMovingChanged: root.log += "moving=" + flick.moving + " "
        onFlickingChanged: root.log += "flicking=" + flick.flicking + " "
        onDraggingChanged: root.log += "dragging=" + flick.dragging + " "
    }

    function out(name) {
        console.log(name, "[" + root.log.trim() + "]", "contentX", flick.contentX.toFixed(2), "vel", flick.horizontalVelocity.toFixed(1))
        root.log = ""
    }
    function wait(n) {
        for (var i = 0; i < n; i++) tc.wait(60)
    }
    function reset() {
        flick.contentX = 0
        root.log = ""
    }

    Timer {
        interval: 200
        running: true
        onTriggered: {
            console.log("defaults", flick.flickDeceleration, flick.maximumFlickVelocity, flick.pressDelay, flick.flickableDirection, flick.boundsBehavior, flick.boundsMovement, flick.rebound !== undefined)
            // 1: slow drag of 100 px (steps of 20 px each 100 ms), pause, release
            reset()
            tc.mousePress(flick, 250, 150)
            for (var i = 1; i <= 5; i++) { tc.mouseMove(flick, 250 - i * 20, 150, 100); out("slow move " + i) }
            wait(3)
            tc.mouseRelease(flick, 150, 150)
            out("slow released")
            wait(10)
            out("slow settled")
            // 2: quick drag of 100 px (steps of 20 px each 10 ms), release at once
            reset()
            tc.mousePress(flick, 250, 150)
            for (var j = 1; j <= 5; j++) { tc.mouseMove(flick, 250 - j * 20, 150, 10); out("fast move " + j) }
            tc.mouseRelease(flick, 150, 150, Qt.LeftButton, Qt.NoModifier, 10)
            out("fast released")
            for (var k = 0; k < 12; k++) { tc.wait(100); out("fast t+" + ((k + 1) * 100)) }
            wait(10)
            out("fast settled")
            // 3: threshold: moves of 1 px
            reset()
            tc.mousePress(flick, 250, 150)
            for (var m = 1; m <= 14; m++) { tc.mouseMove(flick, 250 - m, 150, 20); out("1px move " + m) }
            tc.mouseRelease(flick, 236, 150)
            out("1px released")
            wait(5)
            Qt.quit()
        }
    }
}
