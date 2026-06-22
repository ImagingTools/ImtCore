import QtQuick

Item {
    id: root
    width: 720
    height: 320

    function logFocus(name, obj) {
        console.log(name, "focus", obj.focus, "activeFocus", obj.activeFocus)
    }

    function dump(tag) {
        console.log(tag,
            "A", a1.focus, a1.activeFocus, a2.focus, a2.activeFocus,
            "B", b1.focus, b1.activeFocus, b2.focus, b2.activeFocus,
            "bridge", a2.KeyNavigation.tab === b1, b1.KeyNavigation.backtab === a2
        )
    }

    FocusScope {
        id: scopeA
        x: 20
        y: 20
        width: 320
        height: 240
        focus: true
        onFocusChanged: root.logFocus("scopeA", scopeA)
        onActiveFocusChanged: root.logFocus("scopeA", scopeA)

        TextInput {
            id: a1
            x: 10
            y: 10
            width: 130
            height: 40
            text: "A1"
            focus: true
            KeyNavigation.tab: a2
            onFocusChanged: root.logFocus("a1", a1)
            onActiveFocusChanged: root.logFocus("a1", a1)
        }

        TextEdit {
            id: a2
            x: 10
            y: 70
            width: 200
            height: 80
            text: "A2"
            KeyNavigation.backtab: a1
            onFocusChanged: root.logFocus("a2", a2)
            onActiveFocusChanged: root.logFocus("a2", a2)
        }
    }

    FocusScope {
        id: scopeB
        x: 380
        y: 20
        width: 320
        height: 240
        onFocusChanged: root.logFocus("scopeB", scopeB)
        onActiveFocusChanged: root.logFocus("scopeB", scopeB)

        TextInput {
            id: b1
            x: 10
            y: 10
            width: 130
            height: 40
            text: "B1"
            KeyNavigation.tab: b2
            onFocusChanged: root.logFocus("b1", b1)
            onActiveFocusChanged: root.logFocus("b1", b1)
        }

        TextEdit {
            id: b2
            x: 10
            y: 70
            width: 200
            height: 80
            text: "B2"
            KeyNavigation.backtab: b1
            onFocusChanged: root.logFocus("b2", b2)
            onActiveFocusChanged: root.logFocus("b2", b2)
        }
    }

    Component.onCompleted: {
        dump("focus013-start")

        a1.forceActiveFocus()
        dump("focus013-a1")
        a1.KeyNavigation.tab.forceActiveFocus()
        dump("focus013-a2")

        a2.KeyNavigation.tab = b1
        b1.KeyNavigation.backtab = a2
        dump("focus013-bridge-set")

        a2.KeyNavigation.tab.forceActiveFocus()
        dump("focus013-jump-to-b1")

        b1.KeyNavigation.tab.forceActiveFocus()
        dump("focus013-b2")

        b1.KeyNavigation.backtab.forceActiveFocus()
        dump("focus013-back-to-a2")

        Qt.quit()
    }
}
