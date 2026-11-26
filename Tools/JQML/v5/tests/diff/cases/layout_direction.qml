import QtQuick
import QtQuick.Layouts

// layoutDirection Qt.RightToLeft of RowLayout, Row, Grid, Flow: mirrored in the width (the implicit one without one),
// the side paddings swapped; the cells of a layout start at whole pixels
Item {
    width: 300; height: 300
    RowLayout { id: rl; width: 200; spacing: 10; layoutDirection: Qt.RightToLeft
        Rectangle { id: rl1; implicitWidth: 30; implicitHeight: 10 }
        Rectangle { id: rl2; implicitWidth: 50; implicitHeight: 10; Layout.fillWidth: true } }
    RowLayout { id: rn; width: 200; spacing: 7
        Rectangle { id: rn1; implicitWidth: 31; implicitHeight: 10 }
        Rectangle { id: rn2; implicitWidth: 33; implicitHeight: 10 }
        Rectangle { id: rn3; implicitWidth: 35; implicitHeight: 10 } }
    Row { id: row; layoutDirection: Qt.RightToLeft; spacing: 5; leftPadding: 3; rightPadding: 9
        Rectangle { id: r1; width: 30; height: 10 }
        Rectangle { id: r2; width: 50; height: 10 } }
    Row { id: rowW; width: 120; layoutDirection: Qt.RightToLeft; spacing: 5
        Rectangle { id: w1; width: 30; height: 10 }
        Rectangle { id: w2; width: 50; height: 10 } }
    Grid { id: grid; columns: 2; spacing: 4; layoutDirection: Qt.RightToLeft
        Rectangle { id: g1; width: 30; height: 10 }
        Rectangle { id: g2; width: 50; height: 10 }
        Rectangle { id: g3; width: 20; height: 10 } }
    Flow { id: flow; width: 100; spacing: 4; layoutDirection: Qt.RightToLeft
        Rectangle { id: f1; width: 40; height: 10 }
        Rectangle { id: f2; width: 40; height: 10 }
        Rectangle { id: f3; width: 40; height: 10 } }
    Component.onCompleted: {
        console.log(rl1.x, rl1.width, rl2.x, rl2.width, "|", rn1.x, rn2.x, rn3.x, rn1.width, rn2.width, rn3.width)
        console.log(row.width, r1.x, r2.x, row.effectiveLayoutDirection, "|", w1.x, w2.x)
        console.log(grid.width, g1.x, g2.x, g3.x, g3.y, grid.effectiveLayoutDirection, "|", f1.x, f2.x, f3.x, f3.y, flow.effectiveLayoutDirection)
        row.layoutDirection = Qt.LeftToRight
        row.forceLayout()
        console.log(r1.x, r2.x, row.effectiveLayoutDirection)
        Qt.quit()
    }
}
