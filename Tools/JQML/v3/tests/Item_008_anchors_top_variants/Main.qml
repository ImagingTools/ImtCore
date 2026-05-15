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

    Item { id: pT; width: 10; height: 10; anchors.top: parent.top; anchors.topMargin: 5 }
    Item { id: pB; width: 10; height: 10; anchors.top: parent.bottom; anchors.topMargin: -20 }
    Item { id: pC; width: 10; height: 10; anchors.top: parent.verticalCenter; anchors.topMargin: 10 }

    Item { id: sT; width: 10; height: 10; anchors.top: ref.top; anchors.topMargin: 1 }
    Item { id: sB; width: 10; height: 10; anchors.top: ref.bottom; anchors.topMargin: 2 }
    Item { id: sC; width: 10; height: 10; anchors.top: ref.verticalCenter; anchors.topMargin: 3 }

    Component.onCompleted: {
        console.log("top parent", pT.y, pB.y, pC.y)
        console.log("top sibling", sT.y, sB.y, sC.y)
        Qt.quit()
    }
}
