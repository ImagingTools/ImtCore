import QtQuick
import QtTest

Window {
    id: root
    width: 200
    height: 100
    visible: true

    property int passInner: 0
    property int passOuter: 0
    property int blockInner: 0
    property int blockOuter: 0

    MouseArea {
        id: passOuter
        x: 0
        y: 0
        width: 100
        height: 100
        onClicked: root.passOuter += 1

        MouseArea {
            id: passInner
            anchors.fill: parent
            propagateComposedEvents: true
            onClicked: function(mouse) {
                root.passInner += 1
                mouse.accepted = false
            }
        }
    }

    MouseArea {
        id: blockOuter
        x: 100
        y: 0
        width: 100
        height: 100
        onClicked: root.blockOuter += 1

        MouseArea {
            id: blockInner
            anchors.fill: parent
            propagateComposedEvents: true
            onClicked: root.blockInner += 1
        }
    }

    TestCase {
        id: tc
        name: "MouseAreaPropagate"
        when: windowShown

        function runScenario() {
            root.passInner = 0
            root.passOuter = 0
            root.blockInner = 0
            root.blockOuter = 0

            try {
                mouseClick(passInner, 20, 20, Qt.LeftButton)
            } catch(err) {}

            try {
                mouseClick(blockInner, 20, 20, Qt.LeftButton)
            } catch(err) {}

            console.log("mouse-propagate", root.passInner, root.passOuter, root.blockInner, root.blockOuter)
        }
    }

    Timer {
        interval: 50
        running: true
        repeat: false
        onTriggered: {
            try {
                tc.runScenario()
            } catch(err) {
                console.log("mouse-propagate-error", err)
            }
            Qt.quit()
        }
    }
}
