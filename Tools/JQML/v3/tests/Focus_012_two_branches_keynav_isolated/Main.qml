import QtQuick

Item {
    id: root
    width: 700
    height: 300

    function logFocus(name, obj) {
        console.log(name, "focus", obj.focus, "activeFocus", obj.activeFocus)
    }

    function dump(tag) {
        console.log(tag,
            "A", a1.focus, a1.activeFocus, a2.focus, a2.activeFocus,
            "B", b1.focus, b1.activeFocus, b2.focus, b2.activeFocus,
            "links", a1.KeyNavigation.tab === a2, b1.KeyNavigation.tab === b2
        )
    }

    Item {
        id: branchA
        x: 20
        y: 20
        width: 300
        height: 220
        onFocusChanged: root.logFocus("branchA", branchA)
        onActiveFocusChanged: root.logFocus("branchA", branchA)

        TextInput {
            id: a1
            x: 10
            y: 10
            width: 120
            height: 40
            text: "a1"
            focus: true
            KeyNavigation.tab: a2
            onFocusChanged: root.logFocus("a1", a1)
            onActiveFocusChanged: root.logFocus("a1", a1)
        }

        TextEdit {
            id: a2
            x: 10
            y: 70
            width: 180
            height: 70
            text: "a2"
            KeyNavigation.backtab: a1
            onFocusChanged: root.logFocus("a2", a2)
            onActiveFocusChanged: root.logFocus("a2", a2)
        }
    }

    Item {
        id: branchB
        x: 360
        y: 20
        width: 300
        height: 220
        onFocusChanged: root.logFocus("branchB", branchB)
        onActiveFocusChanged: root.logFocus("branchB", branchB)

        TextInput {
            id: b1
            x: 10
            y: 10
            width: 120
            height: 40
            text: "b1"
            KeyNavigation.tab: b2
            onFocusChanged: root.logFocus("b1", b1)
            onActiveFocusChanged: root.logFocus("b1", b1)
        }

        TextEdit {
            id: b2
            x: 10
            y: 70
            width: 180
            height: 70
            text: "b2"
            KeyNavigation.backtab: b1
            onFocusChanged: root.logFocus("b2", b2)
            onActiveFocusChanged: root.logFocus("b2", b2)
        }
    }

    Component.onCompleted: {
        dump("focus012-start")
        a1.forceActiveFocus()
        dump("focus012-a1")
        a1.KeyNavigation.tab.forceActiveFocus()
        dump("focus012-a2")
        b1.forceActiveFocus()
        dump("focus012-b1")
        b1.KeyNavigation.tab.forceActiveFocus()
        dump("focus012-b2")
        Qt.quit()
    }
}
