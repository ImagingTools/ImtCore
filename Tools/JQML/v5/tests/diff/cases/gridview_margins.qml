import QtQuick

// the margins of a GridView: its range and its start (as ListView)
Item {
    width: 200; height: 200
    GridView {
        id: g
        width: 100; height: 100
        cellWidth: 50; cellHeight: 30
        topMargin: 12; bottomMargin: 6
        model: 20
        delegate: Rectangle { width: 50; height: 30 }
    }
    GridView {
        id: h
        y: 100; width: 100; height: 100
        flow: GridView.FlowTopToBottom
        cellWidth: 30; cellHeight: 50
        leftMargin: 9
        model: 20
        delegate: Rectangle { width: 30; height: 50 }
    }
    Component.onCompleted: {
        console.log(g.contentY, g.atYBeginning, g.originY, g.contentHeight, "|", h.contentX, h.atXBeginning)
        g.contentY = 1000; g.returnToBounds()
        Qt.quit()
    }
}
