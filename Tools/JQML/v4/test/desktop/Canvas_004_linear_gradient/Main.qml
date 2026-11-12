import QtQuick

Item {
    width: 40
    height: 40

    // createLinearGradient(x0, y0, x1, y1) gives a gradient with addColorStop(); it is a valid fillStyle and strokeStyle.
    // A color stop outside of 0..1 or a bad color is an error in Qt, and the gradient object is not a color string.
    Canvas {
        id: canvas
        width: 40
        height: 40
        onPaint: {
            var ctx = getContext("2d")
            console.log("has", typeof ctx.createLinearGradient)
            var g = ctx.createLinearGradient(0, 0, 40, 0)
            console.log("gradient", typeof g, typeof g.addColorStop)
            g.addColorStop(0, "#ff0000")
            g.addColorStop(1, "#0000ff")
            ctx.fillStyle = "#00ff00"
            ctx.fillStyle = g
            console.log("fillStyle is the gradient", ctx.fillStyle === g)
            ctx.fillRect(0, 0, 40, 20)
            ctx.strokeStyle = g
            console.log("strokeStyle is the gradient", ctx.strokeStyle === g)
            ctx.beginPath()
            ctx.moveTo(0, 30)
            ctx.lineTo(40, 30)
            ctx.stroke()
            ctx.fillStyle = "#ff0000"
            console.log("a color replaces it", ctx.fillStyle)
            console.log("painted", true)
            Qt.quit()
        }
        Component.onCompleted: requestPaint()
    }
}
