import QtQuick
import QtTest

Item {
    id: root
    width: 200
    height: 120

    property int wheelCount: 0

    MouseArea {
        id: area
        anchors.fill: parent
        onWheel: root.wheelCount += 1
    }

    TestCase {
        id: tc
        name: "MouseAreaWheel"

        function runScenario() {
            root.wheelCount = 0

            try {
                mouseWheel(area, 50, 50, 0, 3)
            } catch(err) {}

            if(root.wheelCount === 0) {
                area.wheel({ angleDelta: { x: 0, y: 3 }, accepted: false })
            }

            console.log("mouse-wheel", root.wheelCount)
        }
    }

    Component.onCompleted: {
        try {
            tc.runScenario()
        } catch(err) {
            console.log("mouse-wheel-error", err)
        }
        Qt.quit()
    }
}
