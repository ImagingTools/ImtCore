import QtQuick

Item {
    id: root
    width: 360
    height: 180

    function logFocus(name, obj) {
        console.log(name, "focus", obj.focus, "activeFocus", obj.activeFocus)
    }

    function dump(tag) {
        console.log(tag,
            "item", plain.focus, plain.activeFocus,
            "edit", te.focus, te.activeFocus, te.text
        )
    }

    Item {
        id: plain
        x: 10
        y: 10
        width: 120
        height: 60
        focus: true
        onFocusChanged: root.logFocus("plain", plain)
        onActiveFocusChanged: root.logFocus("plain", plain)
    }

    TextEdit {
        id: te
        x: 150
        y: 10
        width: 180
        height: 80
        text: "line1"
        focus: true
        onFocusChanged: root.logFocus("te", te)
        onActiveFocusChanged: root.logFocus("te", te)
    }

    Component.onCompleted: {
        dump("focus004-start")
        plain.forceActiveFocus()
        dump("focus004-item")
        te.forceActiveFocus()
        dump("focus004-edit")
        te.text = "line1\\nline2"
        dump("focus004-text")
        Qt.quit()
    }
}
