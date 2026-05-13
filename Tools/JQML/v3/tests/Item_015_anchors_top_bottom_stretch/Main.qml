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
        x: 10
        width: 10
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.topMargin: 10
        anchors.bottomMargin: 20
    }

    Item {
        id: s
        x: 30
        width: 10
        anchors.top: ref.top
        anchors.bottom: ref.bottom
        anchors.topMargin: 2
        anchors.bottomMargin: 4
    }

    Component.onCompleted: {
        console.log("topBottom parent", p.y, p.height)
        console.log("topBottom sibling", s.y, s.height)
        Qt.quit()
    }
}
