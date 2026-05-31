import QtQuick

BaseLayout {
    id: root

    property alias columnWidth: columnRect.width

    Column {
        id: column
        spacing: gap

        Rectangle {
            id: columnRect
            width: 30
            height: 12
            color: "green"
        }

        Rectangle {
            width: 14
            height: 18
            color: "yellow"
        }
    }

    function layoutSummary() {
        return rowSummary() + ":" + columnRect.width
    }

    Component.onCompleted: {
        console.log("layout-mid", columnWidth, layoutSummary())
    }
}
