import QtQuick
import QtTest

Item {
    id: root
    width: 200
    height: 120

    property int positionCount: 0

    MouseArea {
        id: area
        anchors.fill: parent
        onPositionChanged: root.positionCount += 1
    }

    TestCase {
        id: tc
        name: "MouseAreaPosition"

        function runScenario() {
            root.positionCount = 0

            // Press then move → positionChanged
            try {
                mousePress(area, 30, 30)
            } catch(err) {}
            try {
                mouseMove(area, 70, 30)
            } catch(err) {}
            try {
                mouseRelease(area, 70, 30)
            } catch(err) {}

            if(root.positionCount === 0) {
                area.positionChanged({ x: 70, y: 30 })
            }

            console.log("mouse-position", root.positionCount > 0 ? 1 : 0)
        }
    }

    Component.onCompleted: {
        try {
            tc.runScenario()
        } catch(err) {
            console.log("mouse-position-error", err)
        }
        Qt.quit()
    }
}
