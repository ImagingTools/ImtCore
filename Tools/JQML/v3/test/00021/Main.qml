import QtQuick

Item {
    width: 400
    height: 300

    Rectangle {
        id: inner
        anchors.fill: parent
        color: "orange"

        Component.onCompleted: {
            console.log("inner.width (после fill):", width)
            console.log("inner.height (после fill):", height)
            console.log("parent.width:", parent.width)
            console.log("parent.height:", parent.height)
        }
    }
}