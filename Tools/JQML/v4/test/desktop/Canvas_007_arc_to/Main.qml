import QtQuick

// ctx.arcTo(x1, y1, x2, y2, radius): the rounded corners of PaintedCorner and LicenseTreeCanvas. What it paints is checked in
// test/canvas-arc-to.mjs (the pixels are read there); here the call itself: it exists, answers nothing and does not fail for a path with
// no current point, a zero radius, points on one line or a negative radius.
Item {
    width: 60
    height: 60

    Canvas {
        id: canvas
        width: 60
        height: 60
        onPaint: {
            var ctx = getContext("2d")
            console.log("arcTo type", typeof ctx.arcTo)
            ctx.fillStyle = "#000000"
            ctx.beginPath()
            console.log("returns the context", ctx.arcTo(50, 10, 50, 30, 15) === ctx)
            ctx.moveTo(30, 10)
            ctx.arcTo(50, 10, 50, 30, 15)
            ctx.arcTo(50, 50, 30, 50, 15)
            ctx.arcTo(10, 50, 10, 30, 15)
            ctx.arcTo(10, 10, 30, 10, 15)
            ctx.closePath()
            ctx.fill()
            console.log("filled", true)
            ctx.reset()
            ctx.beginPath()
            ctx.moveTo(10, 10)
            ctx.arcTo(30, 10, 30, 10, 5)
            console.log("degenerate ok", true)
            ctx.lineTo(10, 30)
            ctx.arcTo(10, 30, 10, 30, 0)
            console.log("zero radius ok", true)
            ctx.arcTo(10, 40, 10, 50, 5)
            console.log("one line ok", true)
            try {
                ctx.arcTo(10, 10, 20, 10, -3)
                console.log("negative radius ok")
            } catch (e) {
                console.log("negative radius refused:", e.name, e.message)
            }
            Qt.quit()
        }
        Component.onCompleted: requestPaint()
    }
}
