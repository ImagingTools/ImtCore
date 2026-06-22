import QtQuick

Item {
    id: root
    width: 760
    height: 360

    function logFocus(name, obj) {
        console.log(name, "focus", obj.focus, "activeFocus", obj.activeFocus)
    }

    function dump(tag) {
        console.log(tag,
            "A", aTop.focus, aTop.activeFocus, aMid.focus, aMid.activeFocus, aLeaf.focus, aLeaf.activeFocus,
            "B", bTop.focus, bTop.activeFocus, bMid.focus, bMid.activeFocus, bLeaf.focus, bLeaf.activeFocus
        )
    }

    Item {
        id: aTop
        x: 20
        y: 20
        width: 340
        height: 280
        focus: true
        onFocusChanged: root.logFocus("aTop", aTop)
        onActiveFocusChanged: root.logFocus("aTop", aTop)

        Item {
            id: aMid
            x: 20
            y: 20
            width: 260
            height: 200
            focus: true
            onFocusChanged: root.logFocus("aMid", aMid)
            onActiveFocusChanged: root.logFocus("aMid", aMid)

            Item {
                id: aLeaf
                x: 20
                y: 20
                width: 100
                height: 40
                focus: true
                onFocusChanged: root.logFocus("aLeaf", aLeaf)
                onActiveFocusChanged: root.logFocus("aLeaf", aLeaf)
            }
        }
    }

    Item {
        id: bTop
        x: 400
        y: 20
        width: 340
        height: 280
        onFocusChanged: root.logFocus("bTop", bTop)
        onActiveFocusChanged: root.logFocus("bTop", bTop)

        Item {
            id: bMid
            x: 20
            y: 20
            width: 260
            height: 200
            focus: true
            onFocusChanged: root.logFocus("bMid", bMid)
            onActiveFocusChanged: root.logFocus("bMid", bMid)

            Item {
                id: bLeaf
                x: 20
                y: 20
                width: 100
                height: 40
                focus: true
                onFocusChanged: root.logFocus("bLeaf", bLeaf)
                onActiveFocusChanged: root.logFocus("bLeaf", bLeaf)
            }
        }
    }

    Component.onCompleted: {
        dump("focus014-start")
        aLeaf.forceActiveFocus()
        dump("focus014-aLeaf")
        bLeaf.forceActiveFocus()
        dump("focus014-bLeaf")
        aMid.forceActiveFocus()
        dump("focus014-aMid")
        bMid.forceActiveFocus()
        dump("focus014-bMid")
        Qt.quit()
    }
}
