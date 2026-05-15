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

    Item { id: pL; width: 20; height: 10; anchors.horizontalCenter: parent.left }
    Item { id: pR; width: 20; height: 10; anchors.horizontalCenter: parent.right }
    Item { id: pC; width: 20; height: 10; anchors.horizontalCenter: parent.horizontalCenter }

    Item { id: sL; width: 20; height: 10; anchors.horizontalCenter: ref.left }
    Item { id: sR; width: 20; height: 10; anchors.horizontalCenter: ref.right }
    Item { id: sC; width: 20; height: 10; anchors.horizontalCenter: ref.horizontalCenter }

    Component.onCompleted: {
        console.log("hcenter parent", pL.x, pR.x, pC.x)
        console.log("hcenter sibling", sL.x, sR.x, sC.x)
        Qt.quit()
    }
}
