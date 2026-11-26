import QtQuick

// Grid: columns by default, rows only, columns only, flow TopToBottom, spacing and its own row / column spacing, the
// item alignment, padding, its implicit size
Item {
    width: 400
    height: 400
    Grid { id: g1; Repeater { model: 6; Rectangle { width: 10 + index * 5; height: 20 + (index % 2) * 10 } } }
    Grid { id: g2; y: 100; rows: 2; spacing: 4; Repeater { model: 5; Rectangle { width: 30; height: 10 + index * 3 } } }
    Grid { id: g3; y: 200; columns: 3; rowSpacing: 7; columnSpacing: 2; padding: 5; flow: Grid.TopToBottom
        horizontalItemAlignment: Grid.AlignHCenter; verticalItemAlignment: Grid.AlignVCenter
        Repeater { model: 7; Rectangle { width: 10 + index * 4; height: 10 + index * 2 } } }
    function pos(g) { let out = []; for (let i = 0; i < g.children.length; i++) { let c = g.children[i]; if (c.width) out.push(c.x + "," + c.y) } return out.join(" ") + " | " + g.implicitWidth + "x" + g.implicitHeight }
    Component.onCompleted: {
        g1.forceLayout(); g2.forceLayout(); g3.forceLayout()
        console.log("default", pos(g1)); console.log("rows", pos(g2)); console.log("columns", pos(g3))
        Qt.quit()
    }
}
