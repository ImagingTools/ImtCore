import QtQuick
import QtTest

// A Shortcut activates for its key sequence, whatever item has the focus: the modifiers have to be the same (Shift+a is not "a"),
// Shift+Tab is not Tab, the Enter of the keypad is not Return, an invalid sequence never activates, and a shortcut that is not
// enabled does not. Each line is what one key click activated.
Item {
    id: root
    width: 300
    height: 300

    property var counts: ({})
    function hit(name) { counts[name] = (counts[name] || 0) + 1 }

    Shortcut { sequence: "Escape"; onActivated: root.hit("Escape") }
    Shortcut { sequence: "Return"; onActivated: root.hit("Return") }
    Shortcut { sequence: "Enter"; onActivated: root.hit("Enter") }
    Shortcut { sequence: "Tab"; onActivated: root.hit("Tab") }
    Shortcut { sequence: "Shift+Tab"; onActivated: root.hit("Shift+Tab") }
    Shortcut { sequence: "Backtab"; onActivated: root.hit("Backtab") }
    Shortcut { sequence: "Up"; onActivated: root.hit("Up") }
    Shortcut { sequence: "Down"; onActivated: root.hit("Down") }
    Shortcut { sequence: "Delete"; onActivated: root.hit("Delete") }
    Shortcut { sequence: "F2"; onActivated: root.hit("F2") }
    Shortcut { sequence: "Ctrl+Z"; onActivated: root.hit("Ctrl+Z") }
    Shortcut { sequence: "Ctrl+Shift+Z"; onActivated: root.hit("Ctrl+Shift+Z") }
    Shortcut { sequence: "Ctrl+0"; onActivated: root.hit("Ctrl+0") }
    Shortcut { sequence: "Space"; onActivated: root.hit("Space") }
    Shortcut { sequence: "Backspace"; onActivated: root.hit("Backspace") }
    Shortcut { sequence: "a"; onActivated: root.hit("a") }
    Shortcut { sequence: "Alt+F4"; onActivated: root.hit("Alt+F4") }
    Shortcut { sequence: "ctrl+j"; onActivated: root.hit("ctrl+j") }
    Shortcut { sequence: "Esc"; enabled: false; onActivated: root.hit("Esc (disabled)") }
    Shortcut { sequence: "Del"; onActivated: root.hit("Del") }
    Shortcut { sequence: "Control+Y"; onActivated: root.hit("Control+Y") }
    Shortcut { sequence: "Ctrl+Plus"; onActivated: root.hit("Ctrl+Plus") }
    Shortcut { sequence: "Ctrl++"; onActivated: root.hit("Ctrl++") }
    Shortcut { sequence: "nonsense"; onActivated: root.hit("nonsense") }
    Shortcut { sequence: ""; onActivated: root.hit("empty") }
    Shortcut { sequence: "Shift+F5"; onActivated: root.hit("Shift+F5") }
    Shortcut { sequence: "Ctrl+Left"; onActivated: root.hit("Ctrl+Left") }
    Shortcut { sequence: "Ctrl+/"; onActivated: root.hit("Ctrl+/") }
    Shortcut { sequence: "PgUp"; onActivated: root.hit("PgUp") }
    Shortcut { sequence: "Home"; onActivated: root.hit("Home") }
    Shortcut { sequence: "Ins"; onActivated: root.hit("Ins") }

    Timer {
        interval: 50
        running: true
        onTriggered: tc.run()
    }

    TestCase {
        id: tc
        name: "shortcuts"
        when: windowShown
        function run() {
            var cases = [
                ["Escape", Qt.Key_Escape, 0], ["Return", Qt.Key_Return, 0], ["Enter", Qt.Key_Enter, 0], ["Enter keypad", Qt.Key_Enter, Qt.KeypadModifier],
                ["Tab", Qt.Key_Tab, 0], ["Shift+Tab as Tab", Qt.Key_Tab, Qt.ShiftModifier], ["Backtab", Qt.Key_Backtab, Qt.ShiftModifier],
                ["Up", Qt.Key_Up, 0], ["Down", Qt.Key_Down, 0], ["Delete", Qt.Key_Delete, 0], ["F2", Qt.Key_F2, 0],
                ["Ctrl+Z", Qt.Key_Z, Qt.ControlModifier], ["Ctrl+Shift+Z", Qt.Key_Z, Qt.ControlModifier | Qt.ShiftModifier],
                ["Ctrl+0", Qt.Key_0, Qt.ControlModifier], ["Space", Qt.Key_Space, 0], ["Backspace", Qt.Key_Backspace, 0],
                ["a", Qt.Key_A, 0], ["Shift+a", Qt.Key_A, Qt.ShiftModifier], ["Ctrl+a", Qt.Key_A, Qt.ControlModifier],
                ["Alt+F4", Qt.Key_F4, Qt.AltModifier], ["Ctrl+J", Qt.Key_J, Qt.ControlModifier], ["Ctrl+Y", Qt.Key_Y, Qt.ControlModifier],
                ["Ctrl+Plus", Qt.Key_Plus, Qt.ControlModifier], ["Ctrl+Shift+Plus", Qt.Key_Plus, Qt.ControlModifier | Qt.ShiftModifier],
                ["Shift+F5", Qt.Key_F5, Qt.ShiftModifier], ["F5", Qt.Key_F5, 0], ["Ctrl+Left", Qt.Key_Left, Qt.ControlModifier], ["Left", Qt.Key_Left, 0],
                ["Ctrl+/", Qt.Key_Slash, Qt.ControlModifier], ["PageUp", Qt.Key_PageUp, 0], ["Home", Qt.Key_Home, 0], ["Insert", Qt.Key_Insert, 0],
                ["Shift+Up", Qt.Key_Up, Qt.ShiftModifier], ["Ctrl+Shift+S", Qt.Key_S, Qt.ControlModifier | Qt.ShiftModifier], ["x", Qt.Key_X, 0]
            ]
            for (var i = 0; i < cases.length; i++) {
                root.counts = ({})
                keyClick(cases[i][1], cases[i][2])
                console.log("key", cases[i][0] + ":", Object.keys(root.counts).sort().map(function(k) { return k + "=" + root.counts[k] }).join("; "))
            }
            // a key press activates, the release does not
            root.counts = ({})
            keyPress(Qt.Key_Escape)
            console.log("press Escape:", Object.keys(root.counts).join("; "))
            root.counts = ({})
            keyRelease(Qt.Key_Escape)
            console.log("release Escape:", Object.keys(root.counts).join("; "))
            Qt.quit()
        }
    }
}
