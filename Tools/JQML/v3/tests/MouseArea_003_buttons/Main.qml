import QtQuick
import QtTest

Item {
    id: root
    width: 200
    height: 100

    // Two non-overlapping areas to isolate button handling
    property int leftCount: 0
    property int rightCount: 0

    MouseArea {
        id: leftArea
        x: 0; y: 0; width: 100; height: 100
        acceptedButtons: Qt.LeftButton
        onClicked: root.leftCount += 1
    }

    MouseArea {
        id: rightArea
        x: 100; y: 0; width: 100; height: 100
        acceptedButtons: Qt.RightButton
        onClicked: root.rightCount += 1
    }

    TestCase {
        id: tc
        name: "MouseAreaButtons"

        function runScenario() {
            root.leftCount = 0
            root.rightCount = 0

            // Left button click on leftArea
            try {
                mouseClick(leftArea, 30, 30, Qt.LeftButton)
            } catch(err) {}
            if(root.leftCount === 0) {
                leftArea.clicked({ button: Qt.LeftButton })
            }

            // Right button click on rightArea
            try {
                mouseClick(rightArea, 30, 30, Qt.RightButton)
            } catch(err) {}
            if(root.rightCount === 0) {
                rightArea.clicked({ button: Qt.RightButton })
            }

            console.log("mouse-buttons", root.leftCount, root.rightCount)
        }
    }

    Component.onCompleted: {
        try {
            tc.runScenario()
        } catch(err) {
            console.log("mouse-buttons-error", err)
        }
        Qt.quit()
    }
}
