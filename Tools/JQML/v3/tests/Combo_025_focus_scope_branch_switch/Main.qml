import QtQuick

Item {
    id: root

    Item {
        id: scopeA

        Item {
            id: aLeaf
            property int value: 10
            property bool active: false
            width: 50
            height: 30
        }
    }

    Item {
        id: scopeB

        Item {
            id: bLeaf
            property int value: 20
            property bool active: false
            width: 60
            height: 40
        }
    }

    function dump(tag) {
        console.log(
            tag,
            "A", aLeaf.value, aLeaf.active,
            "B", bLeaf.value, bLeaf.active
        )
    }

    Component.onCompleted: {
        dump("combo25-start")

        aLeaf.active = true
        bLeaf.active = false
        dump("combo25-afterA")

        aLeaf.active = false
        bLeaf.active = true
        dump("combo25-afterB")

        bLeaf.active = false
        aLeaf.active = true
        aLeaf.value = 100
        bLeaf.value = 200
        dump("combo25-afterA2")

        Qt.quit()
    }
}
