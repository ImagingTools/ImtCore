const QtObject = require("../../QtQml/QtObject")
const Bool = require("../../QtQml/Bool")
const StringProp = require("../../QtQml/String")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")
const Icon = require("../../QtQml/Icon")

/**
 * Action — a reusable UI action with text, icon, enabled state, shortcut.
 * Not a visual item; used by MenuItem, Button, ToolButton etc.
 */
class Action extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        text:       { type: StringProp, value: '' },
        icon:       { type: Icon },
        enabled:    { type: Bool, value: true },
        checkable:  { type: Bool, value: false },
        checked:    { type: Bool, value: false },
        shortcut:   { type: Var, value: undefined },

        textChanged:      { type: Signal, args: [] },
        iconChanged:      { type: Signal, args: [] },
        enabledChanged:   { type: Signal, args: [] },
        checkableChanged: { type: Signal, args: [] },
        checkedChanged:   { type: Signal, args: [] },
        shortcutChanged:  { type: Signal, args: [] },

        triggered: { type: Signal, args: ['source'] },
        toggled:   { type: Signal, args: ['checked'] },
    })

    trigger(source) {
        if (!this.enabled) return
        if (this.checkable) {
            this.checked = !this.checked
            this.toggled(this.checked)
        }
        this.triggered(source || null)
    }

    toggle() {
        if (!this.checkable) return
        this.checked = !this.checked
        this.toggled(this.checked)
    }
}

module.exports = Action
