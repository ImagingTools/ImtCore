import QtQuick

// Path.pointAtPercent through lines, curves and arcs; PathPercent maps a percent to a place (QQuickPath)
Item {
    Path {
        id: p1
        startX: 0; startY: 0
        PathLine { x: 100; y: 0 }
        PathPercent { value: 0.25 }
        PathLine { x: 100; y: 300 }
    }
    Path {
        id: p2
        startX: 20; startY: 50
        PathQuad { x: 180; y: 50; controlX: 100; controlY: -30 }
        PathArc { x: 180; y: 150; radiusX: 50; radiusY: 50 }
        PathCubic { x: 20; y: 150; control1X: 120; control1Y: 220; control2X: 80; control2Y: 80 }
    }
    Path {
        id: p3
        startX: 0; startY: 0
        PathAngleArc { centerX: 50; centerY: 50; radiusX: 50; radiusY: 50; startAngle: 0; sweepAngle: 270; moveToStart: false }
    }
    function pt(p, t) { let q = p.pointAtPercent(t); return Math.round(q.x) + "," + Math.round(q.y) }
    Component.onCompleted: {
        let out = []
        for (let t of [0, 0.1, 0.25, 0.4, 0.5, 0.75, 1]) out.push(pt(p1, t))
        console.log(out.join(" "))
        out = []
        for (let t of [0, 0.2, 0.35, 0.5, 0.65, 0.8, 1]) out.push(pt(p2, t))
        console.log(out.join(" "))
        out = []
        for (let t of [0, 0.15, 0.3, 0.6, 1]) out.push(pt(p3, t))
        console.log(out.join(" "))
        Qt.quit()
    }
}
