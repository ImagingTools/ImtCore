import QtQuick

Item {
    id: root
    width: 680
    height: 320

    function logFocus(name, obj) {
        console.log(name, "focus", obj.focus, "activeFocus", obj.activeFocus)
    }

    function dump(tag) {
        console.log(tag,
            "A", aScopeTop.focus, aScopeTop.activeFocus, aScopeInner.focus, aScopeInner.activeFocus, aNode.focus, aNode.activeFocus,
            "B", bScopeTop.focus, bScopeTop.activeFocus, bScopeInner.focus, bScopeInner.activeFocus, bNode.focus, bNode.activeFocus
        )
    }

    FocusScope {
        id: aScopeTop
        x: 20
        y: 20
        width: 300
        height: 250
        focus: true
        onFocusChanged: root.logFocus("aScopeTop", aScopeTop)
        onActiveFocusChanged: root.logFocus("aScopeTop", aScopeTop)

        FocusScope {
            id: aScopeInner
            x: 20
            y: 20
            width: 240
            height: 180
            focus: true
            onFocusChanged: root.logFocus("aScopeInner", aScopeInner)
            onActiveFocusChanged: root.logFocus("aScopeInner", aScopeInner)

            Item {
                id: aNode
                x: 20
                y: 20
                width: 100
                height: 40
                focus: true
                onFocusChanged: root.logFocus("aNode", aNode)
                onActiveFocusChanged: root.logFocus("aNode", aNode)
            }
        }
    }

    FocusScope {
        id: bScopeTop
        x: 360
        y: 20
        width: 300
        height: 250
        onFocusChanged: root.logFocus("bScopeTop", bScopeTop)
        onActiveFocusChanged: root.logFocus("bScopeTop", bScopeTop)

        FocusScope {
            id: bScopeInner
            x: 20
            y: 20
            width: 240
            height: 180
            focus: true
            onFocusChanged: root.logFocus("bScopeInner", bScopeInner)
            onActiveFocusChanged: root.logFocus("bScopeInner", bScopeInner)

            Item {
                id: bNode
                x: 20
                y: 20
                width: 100
                height: 40
                focus: true
                onFocusChanged: root.logFocus("bNode", bNode)
                onActiveFocusChanged: root.logFocus("bNode", bNode)
            }
        }
    }

    Component.onCompleted: {
        dump("focus011-start")
        aNode.forceActiveFocus()
        dump("focus011-a")
        bNode.forceActiveFocus()
        dump("focus011-b")
        aScopeInner.focus = false
        dump("focus011-aInner-off")
        aScopeInner.focus = true
        aNode.forceActiveFocus()
        dump("focus011-aInner-on")
        Qt.quit()
    }
}
