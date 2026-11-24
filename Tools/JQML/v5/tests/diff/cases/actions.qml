import QtQuick
import QtQuick.Controls
import QtTest

// Action, ActionGroup, ButtonGroup: a button with an action (text, checkable, a click triggers it), toggled / triggered,
// exclusive action groups, exclusive and non-exclusive button groups (checkedButton, checkState, clicked), a Menu of
// actions
Item {
    id: root
    width: 400
    height: 400
    property var log: []
    Action { id: copy; text: "Copy"; onTriggered: (source) => log.push("copy triggered " + (source === copyButton)) }
    Action { id: bold; text: "Bold"; checkable: true; onToggled: log.push("bold toggled " + checked); onTriggered: log.push("bold triggered " + checked) }
    ActionGroup { id: alignGroup; Action { id: left; text: "Left"; checkable: true; checked: true } Action { id: right; text: "Right"; checkable: true } }
    Button { id: copyButton; action: copy; onClicked: log.push("button clicked") }
    Button { id: boldButton; y: 50; action: bold }
    ButtonGroup { id: radios; onClicked: (button) => log.push("group clicked " + button.text) }
    RadioButton { id: r1; y: 100; text: "One"; ButtonGroup.group: radios; checked: true }
    RadioButton { id: r2; y: 140; text: "Two"; ButtonGroup.group: radios }
    ButtonGroup { id: checks; exclusive: false; buttons: [c1, c2] }
    CheckBox { id: c1; x: 200; y: 100 }
    CheckBox { id: c2; x: 200; y: 140 }
    Menu { id: menu; Action { text: "First" } Action { text: "Second"; checkable: true } }
    TestCase {
        name: "Actions"; when: windowShown
        function flush(n) { console.log(n + ": " + log.join(", ")); log = [] }
        function test_actions() {
            console.log("buttons", copyButton.text, boldButton.text, boldButton.checkable, copy.enabled, copy.checkable)
            mouseClick(copyButton, 10, 10); flush("click copy")
            mouseClick(boldButton, 10, 10); flush("click bold"); console.log("bold", bold.checked, boldButton.checked)
            bold.trigger(); flush("trigger()"); console.log("bold", bold.checked, boldButton.checked)
            right.checked = true; console.log("action group", left.checked, right.checked, alignGroup.checkedAction === right, alignGroup.actions.length)
            copy.enabled = false; console.log("disabled", copyButton.enabled); copy.enabled = true
        }
        function test_groups() {
            console.log("radios", radios.checkedButton === r1, radios.buttons.length)
            mouseClick(r2, 5, 5); flush("click r2"); console.log("radios", r1.checked, r2.checked, radios.checkedButton === r2)
            console.log("checks", checks.checkState, checks.exclusive)
            c1.checked = true; console.log("one", checks.checkState); c2.checked = true; console.log("both", checks.checkState)
            checks.checkState = Qt.Unchecked; console.log("cleared", c1.checked, c2.checked)
        }
        function test_menu() {
            console.log("menu", menu.count, menu.actionAt(0).text, menu.itemAt(1).text, menu.itemAt(1).checkable)
        }
    }
}
