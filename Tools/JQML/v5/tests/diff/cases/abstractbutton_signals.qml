import QtQuick
import QtQuick.Controls
import QtTest

// AbstractButton: the order of its signals on a click, a press dragged out and back, a checkable button, press and hold,
// a double click; which controls take a press from a MouseArea below them
Item {
    id: root
    width: 400
    height: 300
    property var log: []
    function note(s) { log.push(s) }

    Button {
        id: button
        x: 10; y: 10; width: 100; height: 40
        hoverEnabled: false
        onPressed: root.note("pressed")
        onReleased: root.note("released")
        onClicked: root.note("clicked")
        onCanceled: root.note("canceled")
        onToggled: root.note("toggled")
        onPressAndHold: root.note("pressAndHold")
        onDoubleClicked: root.note("doubleClicked")
        onPressedChanged: root.note("pressedChanged " + pressed)
        onDownChanged: root.note("downChanged " + down)
        onCheckedChanged: root.note("checkedChanged " + checked)
    }

    MouseArea { id: below; x: 200; y: 10; width: 180; height: 280; onPressed: root.note("mousearea pressed") }
    Control { id: plainControl; x: 200; y: 10; width: 50; height: 50 }
    Label { id: label; x: 260; y: 10; width: 50; height: 50; text: "L" }
    Pane { id: pane; x: 200; y: 100; width: 50; height: 50 }
    Frame { id: frame; x: 260; y: 100; width: 50; height: 50 }

    TestCase {
        name: "AbstractButtonSignals"
        when: windowShown
        function flush(name) { console.log(name + ": " + root.log.join(", ")); root.log = [] }

        function test_1_click() {
            mouseClick(button, 50, 20)
            flush("click")
            mousePress(button, 50, 20)
            mouseMove(button, 150, 20)
            mouseMove(button, 50, 20)
            mouseRelease(button, 50, 20)
            flush("out and back")
            mousePress(button, 50, 20)
            mouseMove(button, 150, 20)
            mouseRelease(button, 150, 20)
            flush("released outside")
            button.checkable = true
            mouseClick(button, 50, 20)
            flush("checkable")
            mouseClick(button, 50, 20)
            flush("checkable again")
            button.checkable = false
            button.checked = false
            root.log = []
        }
        function test_2_hold() {
            mousePress(button, 50, 20)
            wait(1000)
            mouseRelease(button, 50, 20)
            flush("hold")
            mouseDoubleClickSequence(button, 50, 20)
            flush("double click")
        }
        function test_3_keys() {
            button.focusPolicy = Qt.StrongFocus
            button.forceActiveFocus()
            keyPress(Qt.Key_Space)
            keyRelease(Qt.Key_Space)
            flush("space")
            keyClick(Qt.Key_Return)
            flush("return")
            keyClick(Qt.Key_Enter)
            flush("enter")
        }
        function test_4_below() {
            for (let c of [plainControl, label, pane, frame]) {
                mouseClick(c, 25, 25)
                flush(c === plainControl ? "Control" : c === label ? "Label" : c === pane ? "Pane" : "Frame")
            }
        }
        function test_5_methods() {
            button.click()
            flush("click()")
            button.checkable = true
            button.toggle()
            flush("toggle()")
            button.animateClick()
            wait(200)
            flush("animateClick()")
        }
    }
}
