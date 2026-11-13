import QtQuick

// A text that does not fit its width is cut with an ellipsis, and the width of the cut text depends on the mode
// (the start, the middle or the end of the text goes).
Item {
    id: root
    width: 300
    height: 100

    Text {
        id: right
        width: 120
        text: "Editing of the admin panel / Changing of the groups"
        elide: Text.ElideRight
    }

    Text {
        id: middle
        y: 20
        width: 120
        text: "Editing of the admin panel / Changing of the groups"
        elide: Text.ElideMiddle
    }

    Text {
        id: left
        y: 40
        width: 120
        text: "Editing of the admin panel / Changing of the groups"
        elide: Text.ElideLeft
    }

    Text {
        id: none
        y: 60
        width: 120
        text: "Editing of the admin panel / Changing of the groups"
    }

    Component.onCompleted: {
        console.log("right", right.contentWidth, right.truncated)
        console.log("middle", middle.contentWidth, middle.truncated)
        console.log("left", left.contentWidth, left.truncated)
        console.log("none", none.contentWidth, none.truncated)
        Qt.quit()
    }
}
