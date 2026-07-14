import QtQuick

Item {
    id: root
    width: 320
    height: 180

    function logFocus(name, obj) {
        console.log(name, "focus", obj.focus, "activeFocus", obj.activeFocus)
    }

    function dump(tag) {
        console.log(tag,
            "a", a.focus, a.activeFocus,
            "b", b.focus, b.activeFocus,
            "c", c.focus, c.activeFocus
        )
    }

    Item {
        id: a
        x: 10
        y: 10
        width: 80
        height: 40
        focus: true
        onFocusChanged: root.logFocus("a", a)
        onActiveFocusChanged: root.logFocus("a", a)
    }

    Item {
        id: b
        x: 110
        y: 10
        width: 80
        height: 40
        focus: true
        onFocusChanged: root.logFocus("b", b)
        onActiveFocusChanged: root.logFocus("b", b)
    }

    Item {
        id: c
        x: 210
        y: 10
        width: 80
        height: 40
        focus: true
        onFocusChanged: root.logFocus("c", c)
        onActiveFocusChanged: root.logFocus("c", c)
    }

    Component.onCompleted: {
        dump("focus001-start")
        a.forceActiveFocus()
        dump("focus001-a")
        b.forceActiveFocus()
        dump("focus001-b")
        c.forceActiveFocus()
        dump("focus001-c")
        Qt.quit()
    }
}
