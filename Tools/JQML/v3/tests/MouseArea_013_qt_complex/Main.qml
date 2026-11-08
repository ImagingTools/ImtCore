import QtQuick
import QtTest

// Nested areas, composed events, modifiers, hiding in a handler, wheel and drag must behave as in Qt
Window {
    id: root
    width: 300
    height: 220
    visible: true

    property var log: []
    property bool childAcceptPress: true
    property bool topAcceptDouble: true
    property bool hideOnPress: false
    property bool acceptWheel: true

    function add(text) {
        let list = root.log
        list.push(text)
        root.log = list
    }

    function point(mouse) {
        return Math.round(mouse.x) + "," + Math.round(mouse.y)
    }

    MouseArea {
        id: parentArea
        width: 200
        height: 150

        onPressed: function(mouse) { root.add("P.pressed:" + root.point(mouse)) }
        onReleased: function(mouse) { root.add("P.released:" + mouse.isClick) }
        onClicked: function(mouse) { root.add("P.clicked:" + root.point(mouse) + ":" + mouse.modifiers) }
        onDoubleClicked: function(mouse) { root.add("P.doubleClicked:" + root.point(mouse)) }
        onWheel: function(wheel) { root.add("P.wheel:" + wheel.angleDelta.y) }

        MouseArea {
            id: childArea
            x: 20
            y: 20
            width: 100
            height: 80
            propagateComposedEvents: true

            onPressed: function(mouse) {
                root.add("C.pressed:" + root.point(mouse) + ":" + mouse.modifiers)
                mouse.accepted = root.childAcceptPress
                if (root.hideOnPress)
                    childArea.visible = false
            }
            onReleased: function(mouse) { root.add("C.released:" + mouse.isClick) }
            onClicked: function(mouse) { root.add("C.clicked:" + mouse.modifiers) }
            onDoubleClicked: function(mouse) {
                root.add("C.doubleClicked")
                mouse.accepted = root.topAcceptDouble
            }
            onCanceled: root.add("C.canceled")
            onPressedChanged: root.add("C.pressedChanged:" + childArea.pressed)
            onVisibleChanged: root.add("C.visible:" + childArea.visible)
            onWheel: function(wheel) {
                root.add("C.wheel:" + wheel.angleDelta.y)
                wheel.accepted = root.acceptWheel
            }
        }
    }

    MouseArea {
        id: hoverOnly
        x: 210
        y: 10
        width: 80
        height: 60
        hoverEnabled: true
        acceptedButtons: Qt.NoButton

        onEntered: root.add("H.entered")
        onExited: root.add("H.exited")
        onPressed: root.add("H.pressed")
        onClicked: root.add("H.clicked")
    }

    Rectangle {
        id: dragTarget
        x: 210
        y: 100
        width: 60
        height: 60

        MouseArea {
            id: dragArea
            anchors.fill: parent
            drag.target: dragTarget
            drag.minimumX: 200
            drag.maximumX: 240

            onPressed: root.add("D.pressed")
            onReleased: function(mouse) { root.add("D.released:" + mouse.isClick + ":" + dragArea.drag.active + ":" + dragTarget.x + "," + dragTarget.y) }
            onClicked: root.add("D.clicked")
        }
    }

    Item {
        id: neutral
        x: 280
        y: 200
        width: 20
        height: 20
    }

    TestCase {
        id: tc
        name: "MouseAreaQtComplex"
        when: false
    }

    function reset() {
        tc.mouseMove(neutral, 10, 10)
        childArea.visible = true
        root.childAcceptPress = true
        root.topAcceptDouble = true
        root.hideOnPress = false
        root.acceptWheel = true
        dragTarget.x = 210
        dragTarget.y = 100
        root.log = []
    }

    function report(name) {
        console.log(name, root.log.join(" "))
    }

    function run() {
        reset()
        tc.mouseClick(childArea, 10, 15)
        report("nestedClick")

        reset()
        root.childAcceptPress = false
        tc.mouseClick(childArea, 10, 15)
        report("nestedReject")

        reset()
        tc.mouseClick(childArea, 10, 15, Qt.LeftButton, Qt.ShiftModifier)
        report("shiftClick")

        reset()
        root.topAcceptDouble = false
        tc.mouseDoubleClickSequence(childArea, 10, 15)
        report("propagateDouble")

        reset()
        root.hideOnPress = true
        tc.mousePress(childArea, 10, 15)
        tc.mouseRelease(childArea, 10, 15)
        report("hideInPressed")

        reset()
        tc.mouseMove(hoverOnly, 10, 10)
        tc.mouseClick(hoverOnly, 10, 10)
        tc.mouseMove(neutral, 10, 10)
        report("noButtonArea")

        reset()
        tc.mouseWheel(childArea, 10, 15, 0, 120)
        report("wheelAccepted")

        reset()
        root.acceptWheel = false
        tc.mouseWheel(childArea, 10, 15, 0, 120)
        report("wheelRejected")

        reset()
        tc.mousePress(dragArea, 10, 10)
        tc.mouseMove(dragArea, 13, 12)
        tc.mouseMove(dragArea, 30, 25)
        tc.mouseMove(dragArea, 90, 40)
        tc.mouseRelease(dragArea, 90, 40)
        report("drag " + dragTarget.x + "," + dragTarget.y)
    }

    Timer {
        id: startTimer
        interval: 100
        onTriggered: {
            try {
                root.run()
            } catch(err) {
                console.log("complex-error", err)
            }
            Qt.quit()
        }
    }

    Component.onCompleted: startTimer.start()
}
