import QtQuick
import QtQuick.Shapes

// QtQuick.Shapes: the defaults of ShapePath and its elements, the implicit size of a Shape from its paths, closed,
// the stops of a gradient set as a list (GradientRectangle of QmlAppTemplate)
Item {
    width: 300; height: 200
    Shape {
        id: tri
        preferredRendererType: Shape.CurveRenderer
        ShapePath {
            id: p1
            startX: 10; startY: 10
            PathLine { x: 110; y: 10 }
            PathLine { id: l2; x: 60; y: 90 }
            PathLine { x: 10; y: 10 }
        }
    }
    Shape {
        id: arc
        x: 150
        ShapePath {
            id: p2
            strokeColor: "#ff0000"; strokeWidth: 6; fillColor: "transparent"; capStyle: ShapePath.RoundCap
            PathAngleArc { id: a1; centerX: 40; centerY: 40; radiusX: 30; radiusY: 30; startAngle: -90; sweepAngle: 270 }
        }
    }
    Shape {
        id: rect
        ShapePath {
            id: p3
            strokeWidth: -1
            fillGradient: LinearGradient { id: lg; x1: 0; y1: 0; x2: 100; y2: 0 }
            PathRectangle { id: pr; width: 100; height: 40; radius: 8; topLeftRadius: 0 }
        }
    }
    Component { id: stopC; GradientStop {} }
    Component.onCompleted: {
        console.log(p1.strokeColor, p1.strokeWidth, p1.fillColor, p1.fillRule, p1.joinStyle, p1.capStyle, p1.miterLimit, p1.strokeStyle, p1.dashOffset, JSON.stringify(p1.dashPattern), p1.fillGradient)
        console.log(tri.implicitWidth, tri.implicitHeight, p1.closed, p2.closed, tri.status, tri.rendererType === Shape.CurveRenderer || tri.rendererType === Shape.GeometryRenderer)
        console.log(arc.implicitWidth, arc.implicitHeight, a1.moveToStart, pr.radius, pr.topLeftRadius, pr.topRightRadius, pr.bottomLeftRadius)
        console.log(rect.implicitWidth, rect.implicitHeight, lg.spread, ShapeGradient.PadSpread, ShapeGradient.RepeatSpread)
        l2.x = 200
        console.log(tri.implicitWidth, tri.implicitHeight)
        lg.stops = [stopC.createObject(lg, { position: 0, color: "red" }), stopC.createObject(lg, { position: 1, color: "blue" })]
        console.log(lg.stops.length, lg.stops[1].color, ShapePath.WindingFill, ShapePath.DashLine, PathArc.Counterclockwise)
        Qt.quit()
    }
}
