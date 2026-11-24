import QtQuick
import QtTest

// A binding of the text of a TextInput stays after the user types: a new value of its source replaces the typed text
Item {
    width: 300; height: 100
    property string source: "abc"
    TextInput { id: t; width: 200; height: 30; text: source }
    TestCase { name: "Bind"; when: windowShown
        function test_a() {
            t.forceActiveFocus(); t.cursorPosition = 3
            keyClick(Qt.Key_D); console.log("typed", t.text, source)
            source = "xyz"; console.log("source changed", t.text)
            keyClick(Qt.Key_Q); source = "zz"; console.log("again", t.text)
            t.text = "own"; source = "after"; console.log("assigned", t.text)
        }
    }
}
