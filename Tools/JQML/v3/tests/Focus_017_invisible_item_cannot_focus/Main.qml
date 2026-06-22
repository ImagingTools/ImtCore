import QtQuick

Item {
    id: root
    width: 380
    height: 180

    function logFocus(name, obj) {
        console.log(name, "focus", obj.focus, "activeFocus", obj.activeFocus, "enabled", obj.enabled, "visible", obj.visible)
    }

    function dump(tag) {
        console.log(tag,
            "target", target.focus, target.activeFocus, target.enabled, target.visible,
            "other", other.focus, other.activeFocus, other.enabled, other.visible
        )
    }

    Item {
        id: target
        x: 10
        y: 10
        width: 120
        height: 40
        visible: false
        onFocusChanged: root.logFocus("target", target)
        onActiveFocusChanged: root.logFocus("target", target)
    }

    Item {
        id: other
        x: 150
        y: 10
        width: 120
        height: 40
        focus: true
        onFocusChanged: root.logFocus("other", other)
        onActiveFocusChanged: root.logFocus("other", other)
    }

    Component.onCompleted: {
        dump("focus017-start")

        target.focus = true
        dump("focus017-set-focus-true")

        target.forceActiveFocus()
        dump("focus017-force-active")

        target.visible = true
        target.forceActiveFocus()
        dump("focus017-visible-force")

        target.visible = false
        dump("focus017-hide-after-focus")

        Qt.quit()
    }
}
