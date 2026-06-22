import QtQuick

Item {
    id: root
    width: 620
    height: 280

    function logFocus(name, obj) {
        console.log(name, "focus", obj.focus, "activeFocus", obj.activeFocus)
    }

    function dump(tag) {
        console.log(tag,
            "A", scopeA.focus, scopeA.activeFocus, aInput.focus, aInput.activeFocus, aEdit.focus, aEdit.activeFocus,
            "B", scopeB.focus, scopeB.activeFocus, bInput.focus, bInput.activeFocus, bEdit.focus, bEdit.activeFocus
        )
    }

    FocusScope {
        id: scopeA
        x: 20
        y: 20
        width: 260
        height: 220
        focus: true
        onFocusChanged: root.logFocus("scopeA", scopeA)
        onActiveFocusChanged: root.logFocus("scopeA", scopeA)

        TextInput {
            id: aInput
            x: 10
            y: 10
            width: 140
            height: 40
            text: "A-in"
            focus: true
            onFocusChanged: root.logFocus("aInput", aInput)
            onActiveFocusChanged: root.logFocus("aInput", aInput)
        }

        TextEdit {
            id: aEdit
            x: 10
            y: 70
            width: 200
            height: 80
            text: "A-edit"
            onFocusChanged: root.logFocus("aEdit", aEdit)
            onActiveFocusChanged: root.logFocus("aEdit", aEdit)
        }
    }

    FocusScope {
        id: scopeB
        x: 320
        y: 20
        width: 260
        height: 220
        onFocusChanged: root.logFocus("scopeB", scopeB)
        onActiveFocusChanged: root.logFocus("scopeB", scopeB)

        TextInput {
            id: bInput
            x: 10
            y: 10
            width: 140
            height: 40
            text: "B-in"
            focus: true
            onFocusChanged: root.logFocus("bInput", bInput)
            onActiveFocusChanged: root.logFocus("bInput", bInput)
        }

        TextEdit {
            id: bEdit
            x: 10
            y: 70
            width: 200
            height: 80
            text: "B-edit"
            onFocusChanged: root.logFocus("bEdit", bEdit)
            onActiveFocusChanged: root.logFocus("bEdit", bEdit)
        }
    }

    Component.onCompleted: {
        dump("focus010-start")
        aInput.forceActiveFocus()
        dump("focus010-aInput")
        bInput.forceActiveFocus()
        dump("focus010-bInput")
        aEdit.forceActiveFocus()
        dump("focus010-aEdit")
        bEdit.forceActiveFocus()
        dump("focus010-bEdit")
        Qt.quit()
    }
}
