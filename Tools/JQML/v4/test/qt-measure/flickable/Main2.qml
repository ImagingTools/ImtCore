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
            var dists = [9, 10, 11, 12, 13, 30]
            for (var q = 0; q < dists.length; q++) {
                reset()
                tc.mousePress(flick, 250, 150)
                tc.mouseMove(flick, 250 - dists[q], 150, 50)
                out("jump " + dists[q])
                tc.mouseMove(flick, 250 - dists[q] - 5, 150, 50)
                out("  then +5")
                tc.mouseMove(flick, 250 - dists[q] - 10, 150, 50)
                out("  then +10")
                tc.mouseRelease(flick, 250 - dists[q] - 10, 150, Qt.LeftButton, Qt.NoModifier, 500)
                out("  released")
                wait(3)
            }
            // wheel
            reset()
            tc.mouseWheel(flick, 150, 150, 0, -120)
            out("wheel up/down 120")
            wait(3)
            out("wheel settled")
            Qt.quit()
        }
    }
}
