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

    Item {
        id: p
        y: 10
        height: 10
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 10
        anchors.rightMargin: 20
    }

    Item {
        id: s
        y: 30
        height: 10
        anchors.left: ref.left
        anchors.right: ref.right
        anchors.leftMargin: 2
        anchors.rightMargin: 3
    }

    Component.onCompleted: {
        console.log("leftRight parent", p.x, p.width)
        console.log("leftRight sibling", s.x, s.width)
        Qt.quit()
    }
}
