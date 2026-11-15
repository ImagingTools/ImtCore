import QtQuick

Item {
    width: 80
    height: 40

    // Canvas.RenderStrategy / RenderTarget are int enums (imtcontrols' GraphicsView keeps one in a
    // `property int`), and the Qt Context2D extensions roundedRect()/ellipse() exist.
    property int strategy: Canvas.Threaded

    Canvas {
        id: canvas
        width: 80
        height: 40
        renderTarget: Canvas.Image
        renderStrategy: strategy
        onPaint: {
            var ctx = getContext("2d")
            ctx.beginPath()
            ctx.roundedRect(4, 4, 72, 32, 6, 6)
            ctx.fillStyle = "#88ccff"
            ctx.fill()
            ctx.beginPath()
            ctx.ellipse(20, 10, 40, 20)
            ctx.fill()
            console.log("enums", Canvas.Image, Canvas.FramebufferObject, Canvas.Immediate, Canvas.Threaded, Canvas.Cooperative)
            console.log("props", renderTarget === Canvas.Image, renderStrategy === Canvas.Threaded, strategy)
            console.log("ext", typeof ctx.roundedRect === "function", typeof ctx.ellipse === "function")
            Qt.quit()
        }
        Component.onCompleted: requestPaint()
    }
}
