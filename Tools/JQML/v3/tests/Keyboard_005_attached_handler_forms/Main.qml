import QtQuick
import QtTest

// Qt takes an attached signal handler as a block, a function expression or an arrow function
Window {
    id: root
    width: 200
    height: 120
    visible: true

    property var log: []

    function add(name) {
        let list = root.log
        list.push(name)
        root.log = list
    }

    Rectangle {
        id: focusRect
        anchors.fill: parent
        focus: true

        Keys.onPressed: function(event) {
            root.add("pressed:" + (event.key === Qt.Key_Tab ? "tab" : event.key === Qt.Key_Return ? "return" : event.key === Qt.Key_Escape ? "escape" : "other"))
        }
        Keys.onTabPressed: function(event) {
            root.add("tab:" + (event.modifiers === Qt.NoModifier))
            event.accepted = true
        }
        Keys.onReturnPressed: (event) => {
            root.add("return")
            event.accepted = true
        }
        Keys.onEscapePressed: {
            root.add("escape")
            event.accepted = true
        }
    }

    Item {
        id: second
        Component.onCompleted: function() {
            root.add("completed")
        }
    }

    TestCase {
        id: tc
        name: "KeyboardAttachedHandlerForms"
        when: false
    }

    Timer {
        id: startTimer
        interval: 100
        onTriggered: {
            focusRect.forceActiveFocus()
            tc.keyClick(Qt.Key_Tab)
            tc.keyClick(Qt.Key_Return)
            tc.keyClick(Qt.Key_Escape)
            console.log("handlers", root.log.join(" "), focusRect.activeFocus)
            Qt.quit()
        }
    }

    Component.onCompleted: startTimer.start()
}
