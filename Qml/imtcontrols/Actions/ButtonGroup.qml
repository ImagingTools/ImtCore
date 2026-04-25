import QtQuick 2.12
import imtcontrols 1.0

// ButtonGroup manages a set of checkable buttons so that only one can be
// checked at a time (when exclusive = true), mirroring the behaviour of
// QtQuick.Controls.ButtonGroup.
//
// Usage:
//   ButtonGroup { id: myGroup }
//   RadioButton { text: "A"; buttonGroup: myGroup }
//   RadioButton { text: "B"; buttonGroup: myGroup }
//
// Buttons can also be registered manually:
//   ButtonGroup { id: myGroup }
//   CheckBox { id: cbA; onCheckedChanged: if(checked) myGroup.addButton(cbA) }

QtObject {
    id: buttonGroup

    // When true only one button in the group may be checked at a time.
    property bool exclusive: true

    // Read-only: the currently checked button (null when none is checked or
    // when exclusive = false).
    property var checkedButton: null

    // The list of registered button objects.
    property var buttons: []

    // Emitted whenever any button in the group is clicked.
    signal clicked(var button)

    // Register a button with this group.
    // Buttons that expose a 'buttonGroup' property (CheckBox, RadioButton
    // derivates) set it directly; this function handles the reverse side.
    function addButton(button) {
        if (!button || buttons.indexOf(button) !== -1)
            return

        buttons.push(button)

        // Wire up state tracking.
        _connectButton(button)
    }

    // Remove a button from the group.
    function removeButton(button) {
        let idx = buttons.indexOf(button)
        if (idx !== -1) {
            buttons.splice(idx, 1)
            _disconnectButton(button)
        }
    }

    // Called by buttons that own a buttonGroup property when their checked
    // state changes (see CheckBox.qml).
    function _onButtonChecked(button) {
        if (!exclusive)
            return

        if (button.checked || button.checkState === Qt.Checked) {
            checkedButton = button
            // Uncheck every other button in the group.
            for (let i = 0; i < buttons.length; i++) {
                let b = buttons[i]
                if (b !== button) {
                    if (b.checkState !== undefined) {
                        b.checkState = Qt.Unchecked
                    } else if (b.checked !== undefined) {
                        b.checked = false
                    }
                }
            }
        } else {
            // If the previously-checked button just became unchecked reset the
            // tracking property.
            if (checkedButton === button)
                checkedButton = null
        }
    }

    // Internal helpers ---------------------------------------------------

    function _connectButton(button) {
        // Use Qt.binding if available; otherwise fall back to signal connections.
        if (button.onClickedChanged !== undefined) {
            button.clicked.connect(function() { buttonGroup.clicked(button) })
        }
        // Register with the button so it knows its group.
        if (button.buttonGroup !== undefined)
            button.buttonGroup = buttonGroup

        // If already checked and exclusive, enforce exclusivity now.
        let isChecked = (button.checked === true) ||
                        (button.checkState !== undefined && button.checkState === Qt.Checked)
        if (exclusive && isChecked)
            _onButtonChecked(button)
    }

    function _disconnectButton(button) {
        if (button.buttonGroup !== undefined && button.buttonGroup === buttonGroup)
            button.buttonGroup = null
        if (checkedButton === button)
            checkedButton = null
    }
}
