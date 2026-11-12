import QtQuick
import QtTest

// Edge cases of MouseArea measured against Qt: hidden in onPressed, released outside without a move, a double click after a rejected press
Window {
    id: root
    width: 260
    height: 200
    visible: true

    property var log: []
    property bool hideOnPress: false
    property bool rejectNextPress: false

    function add(text) {
        let list = root.log
        list.push(text)
        root.log = list
    }

    MouseArea {
        id: area
        x: 20
        y: 20
        width: 100
        height: 80
        pressAndHoldInterval: 100

        onPressed: function(mouse) {
            root.add("pressed")
            if (root.rejectNextPress) {
                root.rejectNextPress = false
                mouse.accepted = false
                root.add("rejected")
            }
            if (root.hideOnPress) {
                area.visible = false
            }
        }
        onPressedChanged: root.add("pressedChanged:" + area.pressed)
        onCanceled: root.add("canceled")
        onReleased: function(mouse) { root.add("released:" + mouse.isClick) }
        onClicked: root.add("clicked")
        onDoubleClicked: root.add("doubleClicked")
        onPressAndHold: root.add("pressAndHold")
        onContainsMouseChanged: root.add("containsMouse:" + area.containsMouse)
    }

    Item {
        id: neutral
        x: 200
        y: 150
        width: 40
        height: 40
    }

    TestCase {
        id: tc
        name: "MouseAreaReviewCases"
        when: false
    }

    function reset() {
        tc.mouseMove(neutral, 10, 10)
        area.visible = true
        root.hideOnPress = false
        root.rejectNextPress = false
        root.log = []
    }

    function report(name) {
        console.log(name, root.log.join(" "))
    }

    Timer {
        id: afterHide
        interval: 300
        onTriggered: {
            root.report("hideInPressed")

            // Released outside without a move after the press
            root.reset()
            tc.mousePress(area, 10, 10)
            tc.mouseRelease(area, 200, 150)
            root.report("releaseOutsideNoMove")

            // A double click whose first press was rejected
            root.reset()
            root.rejectNextPress = true
            tc.mouseDoubleClickSequence(area, 10, 10)
            root.report("doubleClickAfterRejectedPress")

            Qt.quit()
        }
    }

    Timer {
        id: startTimer
        interval: 100
        onTriggered: {
            try {
                root.reset()
                root.hideOnPress = true
                tc.mousePress(area, 10, 10)
                tc.mouseRelease(area, 10, 10)
            } catch(err) {
                console.log("review-cases-error", err)
            }
            afterHide.start()
        }
    }

    Component.onCompleted: startTimer.start()
}
