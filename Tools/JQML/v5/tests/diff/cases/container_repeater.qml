import QtQuick
import QtQuick.Controls

// A Repeater in a SwipeView (a Container): its delegates are the pages, not the Repeater; SwipeView.view of a page;
// a Page whose default property is an alias of the contentItem of a Pane in a Flickable (the gallery's ScrollablePage)
Item {
    width: 360; height: 480
    SwipeView {
        id: view
        width: 300; height: 200
        currentIndex: 1
        Repeater {
            model: 3
            Pane {
                width: SwipeView.view.width
                height: SwipeView.view.height
                property int idx: SwipeView.index
                Label { width: parent.width; wrapMode: Label.Wrap; text: "page " + index }
            }
        }
    }
    Page {
        id: page
        y: 220; width: 360; height: 250
        default property alias content: pane.contentItem
        Flickable {
            anchors.fill: parent
            contentHeight: pane.implicitHeight
            Pane { id: pane; width: parent.width }
        }
        Column {
            id: col
            spacing: 10
            width: parent.width
            Label { id: lab; width: parent.width; wrapMode: Label.Wrap; text: "A long text that should wrap inside the column of the pane of the page" }
        }
    }
    Component.onCompleted: {
        console.log("swipe", view.count, view.itemAt(0).width, view.itemAt(2).height, view.itemAt(1).idx, view.currentItem === view.itemAt(1))
        console.log("page", pane.contentItem === col, col.x, col.y, col.width, lab.width, pane.padding, pane.implicitHeight > 0)
        later.start()
    }
    Timer { id: later; interval: 50; onTriggered: { console.log("laid out", view.currentItem.x, view.itemAt(2).x); Qt.quit() } }
}
