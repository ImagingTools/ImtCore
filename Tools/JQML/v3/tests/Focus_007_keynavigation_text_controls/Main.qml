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
            "input1", input1.focus, input1.activeFocus,
            "input2", input2.focus, input2.activeFocus,
            "edit1", edit1.focus, edit1.activeFocus,
            "nav", input1.KeyNavigation.tab === input2, input2.KeyNavigation.tab === edit1, edit1.KeyNavigation.backtab === input2
        )
    }

    TextInput {
        id: input1
        x: 10
        y: 10
        width: 120
        height: 40
        text: "a"
        focus: true
        KeyNavigation.tab: input2
        onFocusChanged: root.logFocus("input1", input1)
        onActiveFocusChanged: root.logFocus("input1", input1)
    }

    TextInput {
        id: input2
        x: 150
        y: 10
        width: 120
        height: 40
        text: "b"
        KeyNavigation.tab: edit1
        KeyNavigation.backtab: input1
        onFocusChanged: root.logFocus("input2", input2)
        onActiveFocusChanged: root.logFocus("input2", input2)
    }

    TextEdit {
        id: edit1
        x: 10
        y: 70
        width: 260
        height: 80
        text: "c"
        KeyNavigation.backtab: input2
        onFocusChanged: root.logFocus("edit1", edit1)
        onActiveFocusChanged: root.logFocus("edit1", edit1)
    }

    Component.onCompleted: {
        dump("focus007-start")
        input1.forceActiveFocus()
        dump("focus007-input1")
        input1.KeyNavigation.tab.forceActiveFocus()
        dump("focus007-input2")
        input2.KeyNavigation.tab.forceActiveFocus()
        dump("focus007-edit1")
        edit1.KeyNavigation.backtab.forceActiveFocus()
        dump("focus007-backtab")
        Qt.quit()
    }
}
