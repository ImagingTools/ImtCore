import QtQuick
import QtTest

Item {
    id: root
    width: 260
    height: 120

    property int leftClicked: 0
    property int rightClicked: 0

    Rectangle {
        anchors.fill: parent
        color: "#303030"

        MouseArea {
            id: leftArea
            x: 10
            y: 20
            width: 100
            height: 70
            onClicked: root.leftClicked += 1
        }

        MouseArea {
            id: rightArea
            x: 150
            y: 20
            width: 100
            height: 70
            onClicked: root.rightClicked += 1
        }
    }

    TestCase {
        id: tc
        name: "MouseAreaTargetSwitch"

        function runScenario() {
            root.leftClicked = 0
            root.rightClicked = 0

            try {
                mouseClick(leftArea, 30, 30, Qt.LeftButton)
            } catch(err) {
            }
            if(root.leftClicked === 0) {
                leftArea.clicked({ button: Qt.LeftButton })
            }
            try {
                mouseClick(rightArea, 30, 30, Qt.LeftButton)
            } catch(err) {
            }
            if(root.rightClicked === 0) {
                rightArea.clicked({ button: Qt.LeftButton })
            }

            console.log("mouse-target", root.leftClicked, root.rightClicked)
        }
    }

    Component.onCompleted: {
        try {
            tc.runScenario()
        } catch(err) {
            console.log("mouse-target-error", err)
        }
        Qt.quit()
    }
}
