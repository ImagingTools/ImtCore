import QtQuick

Item {
    width: 40
    height: 40

    // A fillStyle/strokeStyle that is not a color (a misspelt property gives undefined) is ignored
    // by Qt's Context2D; the paint handler must go on with the previous style.
    Canvas {
        id: canvas
        width: 40
        height: 40
        property string missing
        onPaint: {
            var ctx = getContext("2d")
            ctx.fillStyle = "#ff0000"
            ctx.strokeStyle = "#00ff00"
            ctx.fillStyle = canvas.nonexistentColor
            ctx.strokeStyle = undefined
            ctx.fillStyle = "nonsense"
            console.log("fill kept", ctx.fillStyle === "#ff0000")
            console.log("stroke kept", ctx.strokeStyle === "#00ff00")
            ctx.fillRect(0, 0, 40, 40)
            console.log("painted", true)
            Qt.quit()
        }
        Component.onCompleted: requestPaint()
    }
}
