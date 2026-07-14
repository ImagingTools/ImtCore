import QtQuick

Item {
    id: root
    width: 320
    height: 200

    function logFocus(name, obj) {
        console.log(name, "focus", obj.focus, "activeFocus", obj.activeFocus)
    }

    function dump(tag) {
        console.log(tag,
            "s1", scope1.focus, scope1.activeFocus, i1.focus, i1.activeFocus,
            "s2", scope2.focus, scope2.activeFocus, i2.focus, i2.activeFocus
        )
    }

    FocusScope {
        id: scope1
        x: 10
        y: 10
        width: 140
        height: 80
        focus: true
        onFocusChanged: root.logFocus("scope1", scope1)
        onActiveFocusChanged: root.logFocus("scope1", scope1)

        Item {
            id: i1
            x: 5
            y: 5
            width: 40
            height: 30
            focus: true
            onFocusChanged: root.logFocus("i1", i1)
            onActiveFocusChanged: root.logFocus("i1", i1)
        }
    }

    FocusScope {
        id: scope2
        x: 170
        y: 10
        width: 140
        height: 80
        focus: true
        onFocusChanged: root.logFocus("scope2", scope2)
        onActiveFocusChanged: root.logFocus("scope2", scope2)

        Item {
            id: i2
            x: 5
            y: 5
            width: 40
            height: 30
            focus: true
            onFocusChanged: root.logFocus("i2", i2)
            onActiveFocusChanged: root.logFocus("i2", i2)
        }
    }

    Component.onCompleted: {
        dump("focus002-start")
        i1.forceActiveFocus()
        dump("focus002-i1")
        i2.forceActiveFocus()
        dump("focus002-i2")
        scope1.focus = false
        dump("focus002-s1-off")
        scope1.focus = true
        i1.forceActiveFocus()
        dump("focus002-s1-on")
        Qt.quit()
    }
}
