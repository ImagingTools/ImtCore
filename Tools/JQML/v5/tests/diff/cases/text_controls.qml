import QtQuick
import QtQuick.Controls
import QtTest

// TextField and TextArea: the background (its colors focused and disabled), the placeholder (place, visibility while
// empty, focused, typed in), typing, the focus reason of a click and of Tab, the font of a control above, insets
Item {
    id: root
    width: 600
    height: 400
    TextField { id: field; x: 10; y: 10; placeholderText: "Name" }
    TextField { id: centered; x: 10; y: 60; placeholderText: "Centered"; horizontalAlignment: TextInput.AlignHCenter }
    TextArea { id: area; x: 250; y: 10; width: 200; height: 100; placeholderText: "Notes" }
    Pane { id: pane; x: 10; y: 120; font.pixelSize: 20; padding: 0
        TextField { id: inner }
    }
    TextField { id: insets; x: 10; y: 200; topInset: 4; leftInset: 6 }
    function p(c) {
        let ph = null
        for (let i = 0; i < c.children.length; i++) if (c.children[i].text === c.placeholderText && c.children[i] !== c.background) ph = c.children[i]
        return ph ? [ph.visible, ph.x, ph.y, ph.width, ph.height, ph.color, ph.elide, ph.verticalAlignment].join(",") : "none"
    }
    function bg(c) { let b = c.background; return b ? [b.visible, b.x, b.y, b.width, b.height, b.color, b.border.width, b.border.color].join(",") : "none" }
    TestCase {
        name: "TextControls"; when: windowShown
        function test_1_field() {
            console.log("field", field.width, field.height, field.padding, field.leftPadding, field.topPadding, field.color, field.selectionColor, field.selectedTextColor, field.verticalAlignment, "| bg", bg(field), "| placeholder", p(field))
            mouseClick(field, 20, 20)
            console.log("clicked", field.activeFocus, field.focusReason, "| bg", bg(field), "| placeholder", p(field))
            keyClick(Qt.Key_A); keyClick(Qt.Key_B)
            console.log("typed", field.text, field.length, "| placeholder", p(field))
            centered.forceActiveFocus(Qt.TabFocusReason)
            console.log("centered focused", centered.focusReason, "| placeholder", p(centered))
            field.enabled = false; console.log("disabled", field.color, "| bg", bg(field)); field.enabled = true
        }
        function test_2_area() {
            console.log("area", area.leftPadding, area.topPadding, area.color, "| bg", bg(area), "| placeholder", p(area), area.placeholderTextColor)
            area.forceActiveFocus(); keyClick(Qt.Key_X)
            console.log("area typed", area.text, "| bg", bg(area), "| placeholder", p(area))
        }
        function test_3_font() {
            console.log("font", inner.font.pixelSize, field.font.pixelSize)
            pane.font.pixelSize = 24; console.log("font changed", inner.font.pixelSize)
            console.log("insets", bg(insets))
        }
    }
}
