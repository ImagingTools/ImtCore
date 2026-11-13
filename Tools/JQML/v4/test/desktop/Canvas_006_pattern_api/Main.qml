import QtQuick

// createPattern(color, Qt.HorPattern) and createPattern(url, repetition): what it returns, what the numbers of the brushes are, and what
// fillStyle / strokeStyle hold when a pattern is set. The picture is not loaded, so the pattern of it draws nothing.
Item {
    width: 40
    height: 40

    Canvas {
        id: canvas
        width: 40
        height: 40
        onPaint: {
            var ctx = getContext("2d")
            console.log("brushes", Qt.NoBrush, Qt.SolidPattern, Qt.Dense1Pattern, Qt.Dense7Pattern, Qt.HorPattern, Qt.VerPattern, Qt.CrossPattern, Qt.BDiagPattern, Qt.FDiagPattern, Qt.DiagCrossPattern)
            var p = ctx.createPattern("#ff0000", Qt.CrossPattern)
            console.log("brush pattern", typeof p)
            ctx.fillStyle = p
            console.log("fillStyle after a pattern", ctx.fillStyle, ctx.fillStyle === p)
            ctx.strokeStyle = p
            console.log("strokeStyle after a pattern", ctx.strokeStyle, ctx.strokeStyle === p)
            ctx.fillStyle = "#123456"
            console.log("a colour replaces it", ctx.fillStyle)
            ctx.save()
            ctx.fillStyle = p
            ctx.restore()
            console.log("restore", ctx.fillStyle)
            var q = ctx.createPattern("not-loaded.png", "repeat")
            console.log("picture pattern", typeof q)
            ctx.fillStyle = q
            console.log("fillStyle after a picture pattern", ctx.fillStyle, ctx.fillStyle === q)
            ctx.fillRect(0, 0, 40, 40)
            console.log("painted", true)
            Qt.quit()
        }
        Component.onCompleted: requestPaint()
    }
}
