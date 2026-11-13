import QtQuick
import QtTest

// Two enabled shortcuts of one sequence activate none of them: one of them gets activatedAmbiguously (on the first key press the one
// that was made last; Qt changes it from press to press, so later presses are only checked for that). A shortcut can be
// switched off and on, its sequence can change, `sequences` listens to several sequences, the visibility of the parent and the context
// do not matter. What the text properties read back, and what happens with an invalid sequence.
Item {
    id: root
    width: 300
    height: 300

    property var counts: ({})
    function hit(name) { counts[name] = (counts[name] || 0) + 1 }

    Shortcut { id: plain; sequence: "Ctrl+Q"; onActivated: root.hit("plain") }
    Shortcut { id: off; sequence: "Ctrl+E"; enabled: false; onActivated: root.hit("off") }
    Shortcut { id: changing; sequence: "Ctrl+W"; onActivated: root.hit("changing") }
    Shortcut { id: many; sequences: ["Ctrl+7", "Alt+7"]; onActivated: root.hit("many") }
    Shortcut { id: both; sequence: "Ctrl+8"; sequences: ["Ctrl+9"]; onActivated: root.hit("both") }
    Shortcut { sequence: "Ctrl+D"; onActivated: root.hit("D first"); onActivatedAmbiguously: root.hit("D first ambiguous") }
    Shortcut { sequence: "Ctrl+D"; onActivated: root.hit("D second"); onActivatedAmbiguously: root.hit("D second ambiguous") }
    Shortcut { id: third; sequence: "Ctrl+T"; onActivated: root.hit("T first"); onActivatedAmbiguously: root.hit("T first ambiguous") }
    Shortcut { sequence: "Ctrl+T"; onActivated: root.hit("T second"); onActivatedAmbiguously: root.hit("T second ambiguous") }
    Shortcut { sequence: "Ctrl+T"; onActivated: root.hit("T third"); onActivatedAmbiguously: root.hit("T third ambiguous") }
    Item {
        visible: false
        Shortcut { sequence: "Ctrl+I"; onActivated: root.hit("invisible parent") }
    }
    Item {
        id: other
        Shortcut { sequence: "Ctrl+O"; onActivated: root.hit("other item") }
    }
    Shortcut { sequence: "Ctrl+J"; context: Qt.ApplicationShortcut; onActivated: root.hit("application") }
    Shortcut { sequence: "Ctrl+K"; context: Qt.WindowShortcut; onActivated: root.hit("window") }
    Shortcut { id: texts1; sequence: "ctrl+z" }
    Shortcut { id: texts2; sequence: "Esc" }
    Shortcut { id: texts3; sequence: "Ctrl++" }
    Shortcut { id: texts4; sequence: "nonsense" }
    Shortcut { id: texts5; sequence: "PgUp" }
    Shortcut { id: texts6; sequence: "Ctrl+Z, Ctrl+Y" }

    Timer {
        interval: 50
        running: true
        onTriggered: tc.run()
    }

    TestCase {
        id: tc
        name: "shortcuts"
        when: windowShown
        function click(label, key, modifiers) {
            root.counts = ({})
            keyClick(key, modifiers)
            console.log("key", label + ":", Object.keys(root.counts).sort().map(function(k) { return k + "=" + root.counts[k] }).join("; "))
        }
        function ambiguity(label, key, modifiers) {
            root.counts = ({})
            keyClick(key, modifiers)
            var names = Object.keys(root.counts)
            var ambiguous = names.filter(function(k) { return k.indexOf("ambiguous") >= 0 }).length
            var activated = names.length - ambiguous
            console.log("key", label + ": ambiguous", ambiguous, "activated", activated)
        }
        function run() {
            console.log("defaults", plain.enabled, plain.autoRepeat, plain.context, JSON.stringify(plain.sequence), JSON.stringify(plain.sequences), "|", plain.nativeText, "|", plain.portableText)
            console.log("contexts", Qt.WidgetShortcut, Qt.WindowShortcut, Qt.ApplicationShortcut, Qt.WidgetWithChildrenShortcut)
            console.log("many", JSON.stringify(many.sequence), JSON.stringify(many.sequences), "|", many.portableText)
            console.log("both", JSON.stringify(both.sequence), JSON.stringify(both.sequences), "|", both.portableText)
            var all = [texts1, texts2, texts3, texts4, texts5, texts6]
            for (var i = 0; i < all.length; i++) {
                console.log("text", JSON.stringify(all[i].sequence), "|", all[i].portableText)
            }
            click("Ctrl+Q", Qt.Key_Q, Qt.ControlModifier)
            click("Ctrl+E (disabled)", Qt.Key_E, Qt.ControlModifier)
            off.enabled = true
            click("Ctrl+E (enabled)", Qt.Key_E, Qt.ControlModifier)
            off.enabled = false
            click("Ctrl+E (disabled again)", Qt.Key_E, Qt.ControlModifier)
            click("Ctrl+W", Qt.Key_W, Qt.ControlModifier)
            changing.sequence = "Ctrl+R"
            click("Ctrl+W after the change", Qt.Key_W, Qt.ControlModifier)
            click("Ctrl+R after the change", Qt.Key_R, Qt.ControlModifier)
            click("Ctrl+7", Qt.Key_7, Qt.ControlModifier)
            click("Alt+7", Qt.Key_7, Qt.AltModifier)
            click("Ctrl+8", Qt.Key_8, Qt.ControlModifier)
            click("Ctrl+9", Qt.Key_9, Qt.ControlModifier)
            click("Ctrl+D", Qt.Key_D, Qt.ControlModifier)
            click("Ctrl+T", Qt.Key_T, Qt.ControlModifier)
            third.enabled = false
            ambiguity("Ctrl+T, the first one off", Qt.Key_T, Qt.ControlModifier)
            ambiguity("Ctrl+T again", Qt.Key_T, Qt.ControlModifier)
            click("Ctrl+I", Qt.Key_I, Qt.ControlModifier)
            click("Ctrl+O", Qt.Key_O, Qt.ControlModifier)
            click("Ctrl+J", Qt.Key_J, Qt.ControlModifier)
            click("Ctrl+K", Qt.Key_K, Qt.ControlModifier)
            Qt.quit()
        }
    }
}
