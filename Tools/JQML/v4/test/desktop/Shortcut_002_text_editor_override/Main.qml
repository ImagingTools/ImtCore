import QtQuick
import QtTest

// A Shortcut gets a key before the item that has the focus, with one exception: a text editor keeps the keys of editing for itself
// (characters, Backspace, Delete, Left, Right, Ctrl+Z, Ctrl+C...), so the shortcut of those does not activate while it has the focus.
// A TextEdit keeps Return, Up, Down and Tab as well, a read-only TextInput keeps nothing. Escape, F2 and Ctrl+S are for the shortcut.
// The line of a key lists what happened: S: a shortcut, K: / A: / T: the Keys.onPressed of an item.
Item {
    id: root
    width: 300
    height: 300

    property var counts: ({})
    function hit(name) { counts[name] = (counts[name] || 0) + 1 }

    Shortcut { sequence: "Escape"; onActivated: root.hit("S:Escape") }
    Shortcut { sequence: "Return"; onActivated: root.hit("S:Return") }
    Shortcut { sequence: "Up"; onActivated: root.hit("S:Up") }
    Shortcut { sequence: "Down"; onActivated: root.hit("S:Down") }
    Shortcut { sequence: "Tab"; onActivated: root.hit("S:Tab") }
    Shortcut { sequence: "Ctrl+Z"; onActivated: root.hit("S:Ctrl+Z") }
    Shortcut { sequence: "Ctrl+C"; onActivated: root.hit("S:Ctrl+C") }
    Shortcut { sequence: "Backspace"; onActivated: root.hit("S:Backspace") }
    Shortcut { sequence: "Delete"; onActivated: root.hit("S:Delete") }
    Shortcut { sequence: "Left"; onActivated: root.hit("S:Left") }
    Shortcut { sequence: "Space"; onActivated: root.hit("S:Space") }
    Shortcut { sequence: "a"; onActivated: root.hit("S:a") }
    Shortcut { sequence: "F2"; onActivated: root.hit("S:F2") }
    Shortcut { sequence: "Ctrl+S"; onActivated: root.hit("S:Ctrl+S") }

    Item {
        id: keyItem
        width: 50
        height: 50
        Keys.onPressed: function(event) { root.hit("K:pressed " + event.key); event.accepted = false }
        Keys.onEscapePressed: root.hit("K:Escape")
    }
    Item {
        id: acceptItem
        width: 50
        height: 50
        Keys.onPressed: function(event) { root.hit("A:pressed " + event.key); event.accepted = true }
    }
    TextInput {
        id: input
        y: 60
        width: 100
        text: "hello"
        Keys.onPressed: function(event) { root.hit("T:pressed " + event.key) }
    }
    TextEdit {
        id: edit
        y: 100
        width: 100
        text: "world"
    }
    Rectangle {
        id: plain
        y: 150
        width: 50
        height: 50
    }

    Timer {
        interval: 50
        running: true
        onTriggered: tc.run()
    }

    TestCase {
        id: tc
        name: "shortcuts"
        when: windowShown
        function go(label, target, keys) {
            target.forceActiveFocus()
            for (var i = 0; i < keys.length; i++) {
                root.counts = ({})
                keyClick(keys[i][1], keys[i][2] || 0)
                var names = Object.keys(root.counts).sort()
                console.log(label, keys[i][0] + ":", names.map(function(k) { return k + "=" + root.counts[k] }).join("; "))
            }
        }
        function run() {
            var keys = [["Escape", Qt.Key_Escape], ["Return", Qt.Key_Return], ["Up", Qt.Key_Up], ["Down", Qt.Key_Down], ["Tab", Qt.Key_Tab],
                        ["Ctrl+Z", Qt.Key_Z, Qt.ControlModifier], ["Ctrl+C", Qt.Key_C, Qt.ControlModifier], ["Backspace", Qt.Key_Backspace],
                        ["Delete", Qt.Key_Delete], ["Left", Qt.Key_Left], ["Space", Qt.Key_Space], ["a", Qt.Key_A], ["F2", Qt.Key_F2],
                        ["Ctrl+S", Qt.Key_S, Qt.ControlModifier], ["x", Qt.Key_X]]
            go("[Rectangle]", plain, keys)
            go("[Item, Keys.onPressed]", keyItem, keys)
            go("[Item, Keys.onPressed accepting]", acceptItem, keys)
            go("[TextInput]", input, keys)
            go("[TextEdit]", edit, keys)
            input.readOnly = true
            go("[read-only TextInput]", input, [["Backspace", Qt.Key_Backspace], ["Left", Qt.Key_Left], ["a", Qt.Key_A], ["Ctrl+C", Qt.Key_C, Qt.ControlModifier]])
            Qt.quit()
        }
    }
}
