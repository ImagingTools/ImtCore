import QtQuick

Item {
    id: root
    width: 600
    height: 260

    function logFocus(name, obj) {
        console.log(name, "focus", obj.focus, "activeFocus", obj.activeFocus)
    }

    function dump(tag) {
        console.log(tag,
            "a", aRoot.focus, aRoot.activeFocus, aLeaf1.focus, aLeaf1.activeFocus, aLeaf2.focus, aLeaf2.activeFocus,
            "b", bRoot.focus, bRoot.activeFocus, bLeaf1.focus, bLeaf1.activeFocus, bLeaf2.focus, bLeaf2.activeFocus
        )
    }

    Item {
        id: aRoot
        x: 20
        y: 20
        width: 250
        height: 180
        focus: true
        onFocusChanged: root.logFocus("aRoot", aRoot)
        onActiveFocusChanged: root.logFocus("aRoot", aRoot)

        Item {
            id: aLeaf1
            x: 10
            y: 10
            width: 90
            height: 40
            focus: true
            onFocusChanged: root.logFocus("aLeaf1", aLeaf1)
            onActiveFocusChanged: root.logFocus("aLeaf1", aLeaf1)
        }

        Item {
            id: aLeaf2
            x: 10
            y: 70
            width: 90
            height: 40
            onFocusChanged: root.logFocus("aLeaf2", aLeaf2)
            onActiveFocusChanged: root.logFocus("aLeaf2", aLeaf2)
        }
    }

    Item {
        id: bRoot
        x: 320
        y: 20
        width: 250
        height: 180
        onFocusChanged: root.logFocus("bRoot", bRoot)
        onActiveFocusChanged: root.logFocus("bRoot", bRoot)

        Item {
            id: bLeaf1
            x: 10
            y: 10
            width: 90
            height: 40
            focus: true
            onFocusChanged: root.logFocus("bLeaf1", bLeaf1)
            onActiveFocusChanged: root.logFocus("bLeaf1", bLeaf1)
        }

        Item {
            id: bLeaf2
            x: 10
            y: 70
            width: 90
            height: 40
            onFocusChanged: root.logFocus("bLeaf2", bLeaf2)
            onActiveFocusChanged: root.logFocus("bLeaf2", bLeaf2)
        }
    }

    Component.onCompleted: {
        dump("focus009-start")
        aLeaf1.forceActiveFocus()
        dump("focus009-a1")
        bLeaf1.forceActiveFocus()
        dump("focus009-b1")
        aLeaf2.forceActiveFocus()
        dump("focus009-a2")
        bLeaf2.forceActiveFocus()
        dump("focus009-b2")
        Qt.quit()
    }
}
