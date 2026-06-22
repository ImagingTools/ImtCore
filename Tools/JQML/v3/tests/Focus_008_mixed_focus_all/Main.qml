import QtQuick

Item {
    id: root
    width: 520
    height: 260

    function logFocus(name, obj) {
        console.log(name, "focus", obj.focus, "activeFocus", obj.activeFocus)
    }

    function dump(tag) {
        console.log(tag,
            "scope", scope.focus, scope.activeFocus,
            "innerItem", innerItem.focus, innerItem.activeFocus,
            "input", input.focus, input.activeFocus,
            "edit", edit.focus, edit.activeFocus,
            "outer", outerItem.focus, outerItem.activeFocus,
            "nav", input.KeyNavigation.tab === edit, edit.KeyNavigation.tab === outerItem, outerItem.KeyNavigation.backtab === edit
        )
    }

    FocusScope {
        id: scope
        x: 10
        y: 10
        width: 340
        height: 220
        focus: true
        onFocusChanged: root.logFocus("scope", scope)
        onActiveFocusChanged: root.logFocus("scope", scope)

        Item {
            id: innerItem
            x: 10
            y: 10
            width: 100
            height: 40
            focus: true
            onFocusChanged: root.logFocus("innerItem", innerItem)
            onActiveFocusChanged: root.logFocus("innerItem", innerItem)
        }

        TextInput {
            id: input
            x: 10
            y: 60
            width: 160
            height: 40
            text: "mix-in"
            KeyNavigation.tab: edit
            onFocusChanged: root.logFocus("input", input)
            onActiveFocusChanged: root.logFocus("input", input)
        }

        TextEdit {
            id: edit
            x: 10
            y: 110
            width: 220
            height: 80
            text: "mix-edit"
            KeyNavigation.tab: outerItem
            KeyNavigation.backtab: input
            onFocusChanged: root.logFocus("edit", edit)
            onActiveFocusChanged: root.logFocus("edit", edit)
        }
    }

    Item {
        id: outerItem
        x: 370
        y: 20
        width: 120
        height: 60
        KeyNavigation.backtab: edit
        onFocusChanged: root.logFocus("outerItem", outerItem)
        onActiveFocusChanged: root.logFocus("outerItem", outerItem)
    }

    Component.onCompleted: {
        dump("focus008-start")
        innerItem.forceActiveFocus()
        dump("focus008-inner")
        input.forceActiveFocus()
        dump("focus008-input")
        input.KeyNavigation.tab.forceActiveFocus()
        dump("focus008-edit")
        edit.KeyNavigation.tab.forceActiveFocus()
        dump("focus008-outer")
        outerItem.KeyNavigation.backtab.forceActiveFocus()
        dump("focus008-backtab")
        Qt.quit()
    }
}
