import QtQuick
import QtTest

Item {
    id: root
    width: 300
    height: 200

    MouseArea {
        id: below
        x: 0; y: 0; width: 300; height: 200
        hoverEnabled: true
    }

    MouseArea {
        id: overlay
        x: 100; y: 0; width: 200; height: 200
        hoverEnabled: true

        MouseArea {
            id: child
            x: 50; y: 50; width: 50; height: 50
            hoverEnabled: true
        }
    }

    TestCase {
        id: tc
        name: "MouseAreaHoverOverlay"

        function runScenario() {
            mouseMove(root, 50, 100)
            console.log("hover-uncovered", below.containsMouse, overlay.containsMouse, child.containsMouse)

            mouseMove(root, 120, 20)
            console.log("hover-covered", below.containsMouse, overlay.containsMouse, child.containsMouse)

            mouseMove(root, 170, 70)
            console.log("hover-child", below.containsMouse, overlay.containsMouse, child.containsMouse)

            mouseMove(root, 50, 100)
            console.log("hover-back", below.containsMouse, overlay.containsMouse, child.containsMouse)
        }
    }

    Component.onCompleted: {
        try {
            tc.runScenario()
        } catch(err) {
            console.log("mouse-hover-overlay-error", err)
        }
        Qt.quit()
    }
}
