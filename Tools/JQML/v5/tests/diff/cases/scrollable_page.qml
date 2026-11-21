import QtQuick
import QtQuick.Controls

// The gallery's ScrollablePage: a Page whose default property is an alias of the contentItem of a Pane in a Flickable;
// the Column of a page is the content item of the Pane (its padding, its width)
Item {
    width: 360; height: 480
    component ScrollablePage: Page {
        default property alias content: pane.contentItem
        property alias pane: pane
        Flickable {
            anchors.fill: parent
            contentHeight: pane.implicitHeight
            flickableDirection: Flickable.AutoFlickIfNeeded
            Pane { id: pane; width: parent.width }
        }
    }
    ScrollablePage {
        id: page
        anchors.fill: parent
        Column {
            id: col
            spacing: 40
            width: parent.width
            Label { id: lab; width: parent.width; wrapMode: Label.Wrap; text: "A long text that should wrap inside the column of the pane of the page" }
        }
    }
    Component.onCompleted: {
        // (the width of the column: 360 in Qt, its binding after the resize of the pane; 336 in v5, the resize last)
        console.log(page.pane.contentItem === col, col.parent === page.pane, col.x, col.y, page.pane.padding, page.pane.width)
        Qt.quit()
    }
}
