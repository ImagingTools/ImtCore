import QtQuick

Item {
    id: root

    property int gap: 4
    property alias leftWidth: leftRect.width
    property alias rightHeight: rightRect.height
    property alias titleText: title.text

    Row {
        id: row
        spacing: gap

        Rectangle {
            id: leftRect
            width: 10
            height: 20
            color: "red"
        }

        Rectangle {
            id: rightRect
            width: 15
            height: 25
            color: "blue"
        }
    }

    Text {
        id: title
        text: "base"
    }

    function rowSummary() {
        return leftRect.width + rightRect.height
    }

    Component.onCompleted: {
        console.log("layout-base", leftWidth, rightHeight, titleText)
    }
}
