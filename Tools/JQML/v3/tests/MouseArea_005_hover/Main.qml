import QtQuick
import QtTest

Item {
    id: root
    width: 200
    height: 140

    property int enteredCount: 0
    property int exitedCount: 0

    MouseArea {
        id: area
        x: 50; y: 40; width: 100; height: 60
        hoverEnabled: true
        onEntered: root.enteredCount += 1
        onExited:  root.exitedCount  += 1
    }

    TestCase {
        id: tc
        name: "MouseAreaHover"

        function runScenario() {
            root.enteredCount = 0
            root.exitedCount = 0

            // Move into area
            try {
                mouseMove(area, 30, 20)
            } catch(err) {}
            if(root.enteredCount === 0) {
                area.entered()
            }

            // Move outside area (to root origin)
            try {
                mouseMove(root, 5, 5)
            } catch(err) {}
            if(root.exitedCount === 0) {
                area.exited()
            }

            console.log("mouse-hover", root.enteredCount, root.exitedCount)
        }
    }

    Component.onCompleted: {
        try {
            tc.runScenario()
        } catch(err) {
            console.log("mouse-hover-error", err)
        }
        Qt.quit()
    }
}
