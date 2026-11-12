import QtQuick
import QtTest

Window {
    id: root
    width: 240
    height: 180
    visible: true

    property var events: []

    function record(name) {
        let list = root.events
        list.push(name)
        root.events = list
    }

    MouseArea {
        id: area
        x: 20
        y: 20
        width: 120
        height: 100

        onPressed: function(mouse) {
            root.record("pressed:" + area.pressed + ":" + mouse.x + "," + mouse.y)
        }
        onPressedChanged: root.record("pressedChanged:" + area.pressed)
        onReleased: function(mouse) {
            root.record("released:" + area.pressed)
        }
        onClicked: function(mouse) {
            root.record("clicked:" + area.pressed)
        }
    }

    Connections {
        target: area

        function onPressed(mouse) {
            root.record("connectionsPressed:" + area.pressed)
        }
    }

    Timer {
        id: startTimer
        interval: 100
        onTriggered: {
            try {
                tc.mousePress(area, 10, 15, Qt.LeftButton, Qt.NoModifier, -1)
                console.log("press", root.events.join(" "))
                root.events = []
                tc.mouseRelease(area, 10, 15, Qt.LeftButton, Qt.NoModifier, -1)
                console.log("release", root.events.filter(function(name) { return name.indexOf("pressed:") === 0 || name.indexOf("connectionsPressed:") === 0 }).length)
            } catch(err) {
                console.log("pressed-signal-error", err)
            }
            Qt.quit()
        }
    }

    // Qt: onPressed and Connections.onPressed run on the press only, before onPressedChanged; the release runs neither of them
    TestCase {
        id: tc
        name: "MouseAreaPressedSignal"
        when: false
    }

    Component.onCompleted: startTimer.start()
}
