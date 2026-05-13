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

    Item { id: pL; width: 10; height: 10; anchors.left: parent.left; anchors.leftMargin: 5 }
    Item { id: pR; width: 10; height: 10; anchors.left: parent.right; anchors.leftMargin: -20 }
    Item { id: pC; width: 10; height: 10; anchors.left: parent.horizontalCenter; anchors.leftMargin: 10 }

    Item { id: sL; width: 10; height: 10; anchors.left: ref.left; anchors.leftMargin: 1 }
    Item { id: sR; width: 10; height: 10; anchors.left: ref.right; anchors.leftMargin: 2 }
    Item { id: sC; width: 10; height: 10; anchors.left: ref.horizontalCenter; anchors.leftMargin: 3 }

    Component.onCompleted: {
        console.log("left parent", pL.x, pR.x, pC.x)
        console.log("left sibling", sL.x, sR.x, sC.x)
        Qt.quit()
    }
}
