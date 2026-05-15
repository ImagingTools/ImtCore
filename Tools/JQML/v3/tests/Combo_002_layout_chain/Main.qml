import QtQuick

MidLayout {
    id: root

    property alias flowWidth: flowRect.width
    property int totalWidth: leftWidth + columnWidth + flowWidth

    Flow {
        id: flow
        spacing: gap

        Rectangle {
            id: flowRect
            width: 7
            height: 8
            color: "purple"
        }

        Rectangle {
            width: 9
            height: 10
            color: "orange"
        }
    }

    Component.onCompleted: {
        console.log("combo2", leftWidth, rightHeight, titleText, columnWidth, flowWidth, totalWidth, rowSummary(), layoutSummary())
        Qt.quit()
    }
}
