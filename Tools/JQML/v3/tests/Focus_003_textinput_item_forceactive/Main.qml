import QtQuick

Item {
    id: root
    width: 360
    height: 140

    function logFocus(name, obj) {
        console.log(name, "focus", obj.focus, "activeFocus", obj.activeFocus)
    }

    function dump(tag) {
        console.log(tag,
            "item", plain.focus, plain.activeFocus,
            "input", ti.focus, ti.activeFocus, ti.text
        )
    }

    Item {
        id: plain
        x: 10
        y: 10
        width: 120
        height: 40
        focus: true
        onFocusChanged: root.logFocus("plain", plain)
        onActiveFocusChanged: root.logFocus("plain", plain)
    }

    TextInput {
        id: ti
        x: 150
        y: 10
        width: 180
        height: 40
        text: "abc"
        focus: true
        onFocusChanged: root.logFocus("ti", ti)
        onActiveFocusChanged: root.logFocus("ti", ti)
    }

    Component.onCompleted: {
        dump("focus003-start")
        plain.forceActiveFocus()
        dump("focus003-item")
        ti.forceActiveFocus()
        dump("focus003-input")
        ti.text = "abc123"
        dump("focus003-text")
        Qt.quit()
    }
}
