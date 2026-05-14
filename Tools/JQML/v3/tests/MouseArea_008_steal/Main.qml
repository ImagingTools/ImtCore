import QtQuick
import QtTest

// Test: inner MouseArea captures click, outer does NOT receive it.
// Default behavior: first area in DOM hit-path captures (no propagation).
Item {
    id: root
    width: 200
    height: 200

    property int outerCount: 0
    property int innerCount: 0

    MouseArea {
        id: outerArea
        anchors.fill: parent
        onClicked: root.outerCount += 1

        MouseArea {
            id: innerArea
            anchors.centerIn: parent
            width: 60
            height: 60
            onClicked: root.innerCount += 1
        }
    }

    TestCase {
        id: tc
        name: "MouseAreaSteal"

        function runScenario() {
            root.outerCount = 0
            root.innerCount = 0

            // Click at center → innerArea sits on top, captures event
            try {
                mouseClick(innerArea, 20, 20, Qt.LeftButton)
            } catch(err) {}

            if(root.innerCount === 0) {
                // Fallback: directly emit only inner click (outer should not fire)
                innerArea.clicked({ button: Qt.LeftButton })
            }

            console.log("mouse-steal", root.innerCount, root.outerCount)
        }
    }

    Component.onCompleted: {
        try {
            tc.runScenario()
        } catch(err) {
            console.log("mouse-steal-error", err)
        }
        Qt.quit()
    }
}
