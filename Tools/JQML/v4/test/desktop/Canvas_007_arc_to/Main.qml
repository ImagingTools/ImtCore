import QtQuick

// ctx.arcTo(x1, y1, x2, y2, radius): the rounded corners of PaintedCorner and LicenseTreeCanvas. What is painted is read with getImageData:
// a filled rounded rectangle has its corners cut (the pixel in the corner is empty, the pixel just inside the arc is full).
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
            ctx.moveTo(30, 10)
            ctx.arcTo(50, 10, 50, 30, 15)
            ctx.arcTo(50, 50, 30, 50, 15)
            ctx.arcTo(10, 50, 10, 30, 15)
            ctx.arcTo(10, 10, 30, 10, 15)
            ctx.closePath()
            ctx.fill()
            var img = ctx.getImageData(0, 0, 60, 60)
            var at = function (x, y) { return img.data[(y * 60 + x) * 4 + 3] > 128 ? "F" : "." }
            console.log("corner cut", at(11, 11), at(48, 11), at(11, 48), at(48, 48))
            console.log("inside the arc", at(18, 18), at(41, 18), at(18, 41), at(41, 41))
            console.log("centre", at(30, 30), "outside", at(5, 30), at(30, 5), at(55, 30), at(30, 55))
            console.log("edges", at(30, 11), at(11, 30), at(48, 30), at(30, 48))

            ctx.reset()
            ctx.beginPath()
            ctx.moveTo(10, 10)
            ctx.arcTo(30, 10, 30, 10, 5)
            console.log("degenerate ok", true)
            ctx.lineTo(10, 30)
            ctx.arcTo(10, 30, 10, 30, 0)
            console.log("zero radius ok", true)
            Qt.quit()
        }
        Component.onCompleted: requestPaint()
    }
}
