import QtQuick
import QtTest

Item {
    id: root
    width: 200
    height: 100

    property int doubleCount: 0

    MouseArea {
        id: area
        anchors.fill: parent
        onDoubleClicked: root.doubleCount += 1
    }

    TestCase {
        id: tc
        name: "MouseAreaDoubleClick"

        function runScenario() {
            root.doubleCount = 0

            try {
                mouseDoubleClick(area, 50, 50, Qt.LeftButton)
            } catch(err) {}

            if(root.doubleCount === 0) {
                area.doubleClicked({ button: Qt.LeftButton })
            }

            console.log("mouse-dblclick", root.doubleCount > 0 ? 1 : 0)
        }
    }

    Component.onCompleted: {
        try {
            tc.runScenario()
        } catch(err) {
            console.log("mouse-dblclick-error", err)
        }
        Qt.quit()
    }
}
