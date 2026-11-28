import QtQuick
Item {
    ListView {
        id: view
        cacheBuffer: 0
        spacing: 2
        delegate: Item { width: 40; height: 20 }
    }
    Timer {
        running: true
        interval: 30
        onTriggered: {
            view.model = 3
            console.log("zero", view.contentHeight, view.currentItem !== null, view.itemAtIndex(0) === null, view.itemAtIndex(1) === null)
            view.width = 100
            view.height = 100
            view.forceLayout()
            console.log("sized", view.contentHeight, view.itemAtIndex(2) !== null)
            Qt.quit()
        }
    }
}
