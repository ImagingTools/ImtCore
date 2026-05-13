import QtQuick

Item {
    width: 300
    height: 300

    Item {
        id: ref
        x: 100
        y: 120
        width: 80
        height: 60
    }

    Item { id: pL; width: 10; height: 10; anchors.right: parent.left; anchors.rightMargin: 1 }
    Item { id: pR; width: 10; height: 10; anchors.right: parent.right; anchors.rightMargin: 2 }
    Item { id: pC; width: 10; height: 10; anchors.right: parent.horizontalCenter; anchors.rightMargin: 3 }

    Item { id: sL; width: 10; height: 10; anchors.right: ref.left; anchors.rightMargin: 1 }
    Item { id: sR; width: 10; height: 10; anchors.right: ref.right; anchors.rightMargin: 2 }
    Item { id: sC; width: 10; height: 10; anchors.right: ref.horizontalCenter; anchors.rightMargin: 3 }

    Component.onCompleted: {
        console.log("right parent", pL.x, pR.x, pC.x)
        console.log("right sibling", sL.x, sR.x, sC.x)
        Qt.quit()
    }
}
