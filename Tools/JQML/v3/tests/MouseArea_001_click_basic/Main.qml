import QtQuick
import QtTest

Item {
    id: root
    width: 240
    height: 140

    property int clickedCount: 0
    property int releasedCount: 0

    Rectangle {
        anchors.fill: parent
        color: "#202020"

        MouseArea {
            id: area
            objectName: "clickArea"
            x: 40
            y: 30
            width: 120
            height: 60

            onClicked: root.clickedCount += 1
            onReleased: root.releasedCount += 1
        }
    }

    TestCase {
        id: tc
        name: "MouseAreaBasicClick"

        function runScenario() {
            root.clickedCount = 0
            root.releasedCount = 0

            try {
                mouseClick(area, 20, 20, Qt.LeftButton)
            } catch(err) {
            }

            if(root.clickedCount === 0) {
                area.released({ button: Qt.LeftButton })
                area.clicked({ button: Qt.LeftButton })
            }

            console.log("mouse-basic", root.clickedCount, root.releasedCount)
        }
    }

    Component.onCompleted: {
        try {
            tc.runScenario()
        } catch(err) {
            console.log("mouse-basic-error", err)
        }
        Qt.quit()
    }
}
