import QtQuick

Item {
    id: root
    width: 260
    height: 120

    Canvas {
        id: canvas
        width: 120
        height: 60
        onPaint: {
            var ctx = getContext("2d")
            ctx.fillStyle = "#225588"
            ctx.fillRect(0, 0, width, height)
            ctx.fillStyle = "#ffffff"
            ctx.fillRect(10, 10, 20, 20)
        }
    }

    Component.onCompleted: {
        canvas.requestPaint()
        console.log("wd123", "canvas-state", canvas.available, canvas.canvasSize, canvas.width, canvas.height)
        Qt.quit()
    }
}
