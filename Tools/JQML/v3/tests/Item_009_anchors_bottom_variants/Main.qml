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

    Item { id: pT; width: 10; height: 10; anchors.bottom: parent.top; anchors.bottomMargin: 1 }
    Item { id: pB; width: 10; height: 10; anchors.bottom: parent.bottom; anchors.bottomMargin: 2 }
    Item { id: pC; width: 10; height: 10; anchors.bottom: parent.verticalCenter; anchors.bottomMargin: 3 }

    Item { id: sT; width: 10; height: 10; anchors.bottom: ref.top; anchors.bottomMargin: 1 }
    Item { id: sB; width: 10; height: 10; anchors.bottom: ref.bottom; anchors.bottomMargin: 2 }
    Item { id: sC; width: 10; height: 10; anchors.bottom: ref.verticalCenter; anchors.bottomMargin: 3 }

    Component.onCompleted: {
        console.log("bottom parent", pT.y, pB.y, pC.y)
        console.log("bottom sibling", sT.y, sB.y, sC.y)
        Qt.quit()
    }
}
