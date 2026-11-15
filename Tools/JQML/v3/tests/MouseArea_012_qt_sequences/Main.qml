import QtQuick
import QtTest

// The signal sequences of MouseArea for real mouse input must be the ones of Qt
Window {
    id: root
    width: 260
    height: 200
    visible: true

    property var log: []
    property bool acceptPress: true
    property bool acceptClick: true
    property bool acceptHold: true

    function add(text) {
        let list = root.log
        list.push(text)
        root.log = list
    }

    function point(mouse) {
        return Math.round(mouse.x) + "," + Math.round(mouse.y)
    }

    MouseArea {
        id: lower
        width: 200
        height: 150
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onPressed: function(mouse) { root.add("L.pressed:" + root.point(mouse) + ":" + mouse.button) }
        onReleased: function(mouse) { root.add("L.released:" + mouse.isClick) }
        onClicked: function(mouse) { root.add("L.clicked:" + root.point(mouse) + ":" + mouse.button) }
        onDoubleClicked: function(mouse) { root.add("L.doubleClicked") }
    }

    MouseArea {
        id: area
        x: 20
        y: 20
        width: 100
        height: 80
        pressAndHoldInterval: 100

        onPressed: function(mouse) {
            root.add("pressed:" + root.point(mouse) + ":" + mouse.button + ":" + area.pressed + ":" + area.containsMouse)
            mouse.accepted = root.acceptPress
        }
        onPressedChanged: root.add("pressedChanged:" + area.pressed)
        onContainsMouseChanged: root.add("containsMouse:" + area.containsMouse)
        onContainsPressChanged: root.add("containsPress:" + area.containsPress)
        onEntered: root.add("entered")
        onExited: root.add("exited")
        onPositionChanged: function(mouse) { root.add("position:" + root.point(mouse) + ":" + area.pressed) }
        onReleased: function(mouse) { root.add("released:" + root.point(mouse) + ":" + mouse.isClick + ":" + mouse.wasHeld + ":" + area.pressed) }
        onClicked: function(mouse) {
            root.add("clicked:" + root.point(mouse) + ":" + mouse.button + ":" + mouse.wasHeld)
            mouse.accepted = root.acceptClick
        }
        onDoubleClicked: function(mouse) { root.add("doubleClicked:" + root.point(mouse) + ":" + area.pressed) }
        onPressAndHold: function(mouse) {
            root.add("pressAndHold:" + root.point(mouse) + ":" + mouse.wasHeld)
            mouse.accepted = root.acceptHold
        }
        onCanceled: root.add("canceled:" + area.pressed)
    }

    Item {
        id: neutral
        x: 230
        y: 170
        width: 20
        height: 20
    }

    TestCase {
        id: tc
        name: "MouseAreaQtSequences"
        when: false
    }

    function reset() {
        tc.mouseMove(neutral, 10, 10)
        area.hoverEnabled = false
        area.propagateComposedEvents = false
        area.acceptedButtons = Qt.LeftButton
        area.enabled = true
        area.visible = true
        root.acceptPress = true
        root.acceptClick = true
        root.acceptHold = true
        root.log = []
    }

    function report(name) {
        console.log(name, root.log.join(" "))
    }

    function runSync() {
        reset()
        tc.mousePress(area, 10, 15)
        tc.mouseRelease(area, 10, 15)
        report("click")

        reset()
        tc.mousePress(area, 10, 15)
        tc.mouseMove(area, 150, 15)
        tc.mouseRelease(area, 150, 15)
        report("releaseOutside")

        reset()
        tc.mousePress(area, 10, 15)
        tc.mouseMove(area, 150, 15)
        tc.mouseMove(area, 12, 18)
        tc.mouseRelease(area, 12, 18)
        report("outAndBack")

        reset()
        root.acceptPress = false
        tc.mousePress(area, 10, 15)
        tc.mouseRelease(area, 10, 15)
        report("rejectPress")

        reset()
        tc.mousePress(area, 10, 15, Qt.RightButton)
        tc.mouseRelease(area, 10, 15, Qt.RightButton)
        report("rightIgnored")

        reset()
        area.acceptedButtons = Qt.LeftButton | Qt.RightButton
        tc.mousePress(area, 10, 15, Qt.RightButton)
        tc.mouseRelease(area, 10, 15, Qt.RightButton)
        report("rightAccepted")

        reset()
        area.hoverEnabled = true
        tc.mouseMove(area, 5, 6)
        tc.mouseMove(area, 20, 21)
        tc.mousePress(area, 20, 21)
        tc.mouseRelease(area, 20, 21)
        tc.mouseMove(neutral, 10, 10)
        report("hover")

        reset()
        tc.mouseMove(area, 5, 6)
        tc.mouseMove(area, 20, 21)
        report("noHoverMove")

        reset()
        area.propagateComposedEvents = true
        root.acceptClick = false
        tc.mouseClick(area, 10, 15)
        report("propagateClick")

        reset()
        tc.mouseDoubleClickSequence(area, 10, 15)
        report("doubleClick")

        reset()
        tc.mouseClick(area, 10, 15)
        tc.mouseClick(area, 10, 15)
        report("twoClicks")

        reset()
        tc.mousePress(area, 10, 15)
        area.enabled = false
        tc.mouseRelease(area, 10, 15)
        report("disableWhilePressed")

        reset()
        tc.mousePress(area, 10, 15)
        area.visible = false
        tc.mouseRelease(area, 10, 15)
        report("hideWhilePressed")
    }

    Timer {
        id: holdTimer
        interval: 300
        onTriggered: {
            tc.mouseRelease(area, 10, 15)
            root.report("pressAndHold")
            root.reset()
            root.acceptHold = false
            tc.mousePress(area, 10, 15)
            ignoredHoldTimer.start()
        }
    }

    Timer {
        id: ignoredHoldTimer
        interval: 300
        onTriggered: {
            tc.mouseRelease(area, 10, 15)
            root.report("pressAndHoldIgnored")
            Qt.quit()
        }
    }

    Timer {
        id: startTimer
        interval: 100
        onTriggered: {
            try {
                root.runSync()
            } catch(err) {
                console.log("sequences-error", err)
            }
            root.reset()
            tc.mousePress(area, 10, 15)
            holdTimer.start()
        }
    }

    Component.onCompleted: startTimer.start()
}
