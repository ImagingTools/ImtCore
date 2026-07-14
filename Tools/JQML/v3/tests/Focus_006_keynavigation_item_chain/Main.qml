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
            "a", a.focus, a.activeFocus,
            "b", b.focus, b.activeFocus,
            "c", c.focus, c.activeFocus,
            "nav", a.KeyNavigation.tab === b, b.KeyNavigation.tab === c, c.KeyNavigation.backtab === b
        )
    }

    Item {
        id: a
        x: 10
        y: 10
        width: 90
        height: 40
        focus: true
        KeyNavigation.tab: b
        onFocusChanged: root.logFocus("a", a)
        onActiveFocusChanged: root.logFocus("a", a)
    }

    Item {
        id: b
        x: 130
        y: 10
        width: 90
        height: 40
        KeyNavigation.tab: c
        KeyNavigation.backtab: a
        onFocusChanged: root.logFocus("b", b)
        onActiveFocusChanged: root.logFocus("b", b)
    }

    Item {
        id: c
        x: 250
        y: 10
        width: 90
        height: 40
        KeyNavigation.backtab: b
        onFocusChanged: root.logFocus("c", c)
        onActiveFocusChanged: root.logFocus("c", c)
    }

    Component.onCompleted: {
        dump("focus006-start")
        a.forceActiveFocus()
        dump("focus006-a")
        a.KeyNavigation.tab.forceActiveFocus()
        dump("focus006-a-tab")
        b.KeyNavigation.tab.forceActiveFocus()
        dump("focus006-b-tab")
        c.KeyNavigation.backtab.forceActiveFocus()
        dump("focus006-c-backtab")
        Qt.quit()
    }
}
