import QtQuick 2.12
import imtcontrols 1.0

// ButtonGroup manages a set of checkable buttons so that only one can be
// checked at a time (when exclusive = true), mirroring the behaviour of
// QtQuick.Controls.ButtonGroup.
//
// Usage:
//   ButtonGroup { id: myGroup }
//   CheckBox { text: "A"; buttonGroup: myGroup }
//   CheckBox { text: "B"; buttonGroup: myGroup }
//
// Buttons can also be registered manually:
//   ButtonGroup { id: myGroup }
//   CheckBox { id: cbA; Component.onCompleted: myGroup.addButton(cbA) }

QtObject {
    id: buttonGroup

    // When true only one button in the group may be checked at a time.
    property bool exclusive: true

    // Read-only: the currently checked button (null when none is checked or
    // when exclusive = false).
    property var checkedButton: null

    // The registered button objects. Reassignment triggers QML change
    // notifications so bindings on 'buttons' update correctly.
    property var buttons: []

    // Emitted whenever a button in the group emits its clicked() signal.
    signal clicked(var button)

    // Register a button with this group.
    function addButton(button) {
        if (!button || buttons.indexOf(button) !== -1)
            return

        // Reassign so QML property-change notification fires.
        buttons = buttons.concat([button])

        // Tell the button which group it belongs to (CheckBox, RadioButton etc.)
        if (button.buttonGroup !== undefined && button.buttonGroup !== buttonGroup)
            button.buttonGroup = buttonGroup

        // Enforce exclusivity for any already-checked button.
        let isChecked = (button.checkState !== undefined && button.checkState === Qt.Checked) ||
                        (button.checkState === undefined && button.checked === true)
        if (exclusive && isChecked)
            _onButtonChecked(button)
    }

    // Remove a button from the group.
    function removeButton(button) {
        let idx = buttons.indexOf(button)
        if (idx === -1)
            return

        let arr = buttons.slice()
        arr.splice(idx, 1)
        buttons = arr

        if (button.buttonGroup !== undefined && button.buttonGroup === buttonGroup)
            button.buttonGroup = null

        if (checkedButton === button)
            checkedButton = null
    }

    // Called by buttons when their checkState changes (see CheckBox.qml
    // onCheckStateChanged).  Do not call directly – use the buttonGroup
    // property on the button instead.
    function _onButtonChecked(button) {
        if (!exclusive)
            return

        let isChecked = (button.checkState !== undefined)
                            ? (button.checkState === Qt.Checked)
                            : (button.checked === true)

        if (isChecked) {
            checkedButton = button
            // Uncheck every other registered button.
            for (let i = 0; i < buttons.length; i++) {
                let b = buttons[i]
                if (b === button)
                    continue
                if (b.checkState !== undefined) {
                    if (b.checkState !== Qt.Unchecked)
                        b.checkState = Qt.Unchecked
                } else if (b.checked !== undefined) {
                    if (b.checked)
                        b.checked = false
                }
            }
        } else {
            if (checkedButton === button)
                checkedButton = null
        }
    }
}
