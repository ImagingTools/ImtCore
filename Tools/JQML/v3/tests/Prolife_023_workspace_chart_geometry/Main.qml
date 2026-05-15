import QtQuick 2.0

// Mirrors WorkspacePage chartsBlock geometry formulas:
//   chartWidth = (width - 4*spacing) / 3
//   rowHeight  = height - topRow.height - 2*spacing  (no rows visible)
//              = (height - topRow.height - 4*spacing)/2  (both rows visible)
//              = height - topRow.height - 3*spacing  (one row visible)

Item {
    id: root
    width: 1200
    height: 760

    Item {
        id: chartsBlock
        anchors.fill: parent

        property int spacing: 16
        property int topRowHeight: 110

        property real chartWidth: (width - 4*spacing) / 3
        property real rowHeightNone: height - topRowHeight - 2*spacing
        property real rowHeightBoth: (height - topRowHeight - 4*spacing) / 2
        property real rowHeightOne:  height - topRowHeight - 3*spacing

        property real row1width: chartWidth
        property real row2width: chartWidth
    }

    Component.onCompleted: {
        var cw = chartsBlock.chartWidth
        var rhNone = chartsBlock.rowHeightNone
        var rhBoth = chartsBlock.rowHeightBoth
        var rhOne  = chartsBlock.rowHeightOne

        console.log(
            "prolife23",
            root.width,
            root.height,
            chartsBlock.spacing,
            Math.round(cw),
            Math.round(rhNone),
            Math.round(rhBoth),
            Math.round(rhOne),
            Math.round(chartsBlock.row1width),
            Math.round(chartsBlock.row2width)
        )
        Qt.quit()
    }
}
