import QtQuick

Item {
    id: root
    width: 420
    height: 220

    function logFocus(name, obj) {
        console.log(name, "focus", obj.focus, "activeFocus", obj.activeFocus)
    }

    function dump(tag) {
        console.log(tag,
            "scope", scope.focus, scope.activeFocus,
            "input", ti.focus, ti.activeFocus,
            "edit", te.focus, te.activeFocus
        )
    }

    FocusScope {
        id: scope
        x: 10
        y: 10
        width: 390
        height: 190
        focus: true
        onFocusChanged: root.logFocus("scope", scope)
        onActiveFocusChanged: root.logFocus("scope", scope)

        TextInput {
            id: ti
            x: 10
            y: 10
            width: 180
            height: 40
            text: "in"
            focus: true
            onFocusChanged: root.logFocus("ti", ti)
            onActiveFocusChanged: root.logFocus("ti", ti)
        }

        TextEdit {
            id: te
            x: 10
            y: 70
            width: 280
            height: 80
            text: "edit"
            focus: true
            onFocusChanged: root.logFocus("te", te)
            onActiveFocusChanged: root.logFocus("te", te)
        }
    }

    Component.onCompleted: {
        dump("focus005-start")
        ti.forceActiveFocus()
        dump("focus005-ti")
        te.forceActiveFocus()
        dump("focus005-te")
        scope.focus = false
        dump("focus005-scope-off")
        scope.focus = true
        ti.forceActiveFocus()
        dump("focus005-scope-on")
        Qt.quit()
    }
}
