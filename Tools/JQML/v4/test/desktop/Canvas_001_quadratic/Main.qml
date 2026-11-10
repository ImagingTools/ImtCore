import QtQuick

Item {
    width: 80
    height: 40

    Canvas {
        id: canvas
        width: 80
        height: 40
        onPaint: {
            var ctx = getContext("2d")
            ctx.beginPath()
            ctx.moveTo(0, 40)
            ctx.quadraticCurveTo(20, 0, 40, 0)
            ctx.lineTo(60, 0)
            ctx.quadraticCurveTo(80, 0, 80, 40)
            ctx.closePath()
            ctx.fillStyle = "#88ccff"
            ctx.fill()
            console.log("ok", true)
            console.log("bez", typeof ctx.bezierCurveTo === "function")
            Qt.quit()
        }
        Component.onCompleted: requestPaint()
    }
}
