import QtQuick
import QtQuick.Controls

// The Basic style of the buttons in their states (normal, down, checked, highlighted, flat, disabled, focused by Tab): the
// colors, visibility and borders of the background, the content and the indicator, the place of the indicator and of the
// switch handle. No text: the sizes of a text depend on the fonts.
Item {
    id: root
    width: 800
    height: 600

    Button { id: button }
    ToolButton { id: toolButton }
    RoundButton { id: roundButton; width: 60; height: 30 }
    CheckBox { id: checkBox }
    RadioButton { id: radioButton }
    Switch { id: switchControl }
    DelayButton { id: delayButton }
    TabButton { id: tabButton }
    ItemDelegate { id: itemDelegate }
    CheckDelegate { id: checkDelegate; width: 200 }
    RadioDelegate { id: radioDelegate; width: 200 }
    SwitchDelegate { id: switchDelegate; width: 200 }
    Label { id: label }

    function rect(r) {
        if (!r) return "none"
        return [r.visible, r.color, r.border.width, r.border.color, r.opacity, r.radius, r.x, r.y, r.width, r.height].join(",")
    }
    function content(c) {
        let ci = c.contentItem
        return ci ? [ci.color, ci.x, ci.y, ci.width, ci.height].join(",") : "none"
    }
    function indicator(c) {
        let i = c.indicator
        if (!i) return "none"
        let parts = [i.x, i.y, i.width, i.height, i.color, i.border.width, i.border.color, i.radius]
        for (let k = 0; k < i.children.length; k++) {
            let ch = i.children[k]
            parts.push("[" + [ch.visible, ch.x, ch.y, ch.width, ch.height, ch.color !== undefined ? ch.color : "-"].join(",") + "]")
        }
        return parts.join(",")
    }
    function show(name, c) {
        console.log(name, "bg", rect(c.background), "| content", content(c), "| indicator", indicator(c))
    }
    function states(name, c) {
        show(name, c)
        c.down = true; show(name + " down", c); c.down = undefined
        if (c.checkable !== undefined) { let was = c.checkable; c.checked = true; show(name + " checked", c); c.checked = false; c.checkable = was }
        if (c.highlighted !== undefined) { c.highlighted = true; show(name + " highlighted", c); c.highlighted = false }
        if (c.flat !== undefined) { c.flat = true; show(name + " flat", c); c.down = true; show(name + " flat down", c); c.down = undefined; c.flat = false }
        c.enabled = false; show(name + " disabled", c); c.enabled = true
        c.focusPolicy = Qt.StrongFocus
        c.forceActiveFocus(Qt.TabFocusReason); show(name + " tab focus " + c.visualFocus + " " + c.focusReason, c)
        c.forceActiveFocus(Qt.MouseFocusReason)
        root.forceActiveFocus()
        c.forceActiveFocus(Qt.MouseFocusReason); show(name + " mouse focus " + c.visualFocus + " " + c.focusReason, c)
        root.forceActiveFocus()
    }

    Component.onCompleted: {
        let all = [["Button", button], ["ToolButton", toolButton], ["RoundButton", roundButton], ["CheckBox", checkBox],
                   ["RadioButton", radioButton], ["Switch", switchControl], ["DelayButton", delayButton], ["TabButton", tabButton],
                   ["ItemDelegate", itemDelegate], ["CheckDelegate", checkDelegate], ["RadioDelegate", radioDelegate],
                   ["SwitchDelegate", switchDelegate]]
        for (let i = 0; i < all.length; i++) states(all[i][0], all[i][1])
        checkBox.checkState = Qt.PartiallyChecked; show("CheckBox partial", checkBox)
        checkDelegate.checkState = Qt.PartiallyChecked; show("CheckDelegate partial", checkDelegate)
        switchControl.position = 0.5; show("Switch half", switchControl)
        console.log("Label", label.color, label.linkColor, label.background)
        label.enabled = false; console.log("Label disabled", label.color)
        Qt.quit()
    }
}
