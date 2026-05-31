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

    Item { id: pT; width: 10; height: 20; anchors.verticalCenter: parent.top }
    Item { id: pB; width: 10; height: 20; anchors.verticalCenter: parent.bottom }
    Item { id: pC; width: 10; height: 20; anchors.verticalCenter: parent.verticalCenter }

    Item { id: sT; width: 10; height: 20; anchors.verticalCenter: ref.top }
    Item { id: sB; width: 10; height: 20; anchors.verticalCenter: ref.bottom }
    Item { id: sC; width: 10; height: 20; anchors.verticalCenter: ref.verticalCenter }

    Component.onCompleted: {
        console.log("vcenter parent", pT.y, pB.y, pC.y)
        console.log("vcenter sibling", sT.y, sB.y, sC.y)
        Qt.quit()
    }
}
