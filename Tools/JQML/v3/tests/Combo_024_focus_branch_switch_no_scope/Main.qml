import QtQuick

Item {
    id: root

    Item {
        id: branchA

        Item {
            id: itemA
            property int value: 10
            width: 50
            height: 30
        }
    }

    Item {
        id: branchB

        Item {
            id: itemB
            property int value: 20
            width: 60
            height: 40
        }
    }

    function dump(tag) {
        console.log(
            tag,
            "A", itemA.value, itemA.width, itemA.height,
            "B", itemB.value, itemB.width, itemB.height
        )
    }

    Component.onCompleted: {
        dump("combo24-start")

        itemA.value = 100
        itemA.width = 55
        itemB.value = 200
        itemB.height = 45
        dump("combo24-modified")

        itemA.visible = false
        itemB.visible = true
        dump("combo24-visibility")

        Qt.quit()
    }
}