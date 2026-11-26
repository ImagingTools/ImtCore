import QtQuick

// Canvas and Context2D against Qt 6.12 (CanvasWeb.test.js): drawImage of another Canvas (at its size, scaled, a part
// of it), its errors, of an image data, with a shadow and with globalAlpha.
// The expected lines are CanvasImages.qt.txt (qml -platform offscreen of Qt 6.12).
Item {
    id: root
    width: 200; height: 200
    property int step: 0
    function px(ctx, x, y) {
        let d = ctx.getImageData(x, y, 1, 1).data
        return d[0] + "," + d[1] + "," + d[2] + "," + d[3]
    }
    function tryIt(name, f) {
        try { console.log(name, "ok", f()) } catch (e) { console.log(name, "throws", e.code, e.message) }
    }
    Canvas {
        id: other
        width: 20; height: 20
        onPaint: { let c = getContext("2d"); c.fillStyle = "#00ff00"; c.fillRect(0, 0, 20, 20); c.fillStyle = "#0000ff"; c.fillRect(10, 0, 10, 10) }
    }
    Canvas {
        id: canvas
        width: 100; height: 80
        onPaint: {
            root.step++
            let ctx = getContext("2d")
            if (root.step !== 2) return
            tryIt("drawImage canvas", () => { let r = ctx.drawImage(other, 5, 5); return (r === ctx) + " " + root.px(ctx, 10, 10) + " " + root.px(ctx, 20, 6) })
            tryIt("drawImage scaled", () => { ctx.drawImage(other, 30, 5, 40, 10); return root.px(ctx, 40, 12) + " " + root.px(ctx, 65, 7) + " " + root.px(ctx, 65, 16) })
            tryIt("drawImage source", () => { ctx.drawImage(other, 10, 0, 10, 10, 80, 0, 5, 5); return root.px(ctx, 82, 2) + " " + root.px(ctx, 86, 2) })
            tryIt("drawImage index", () => ctx.drawImage(other, 0, 0, 30, 10, 0, 0, 5, 5))
            tryIt("drawImage negsrc", () => ctx.drawImage(other, -1, 0, 5, 5, 0, 0, 5, 5))
            tryIt("drawImage zero", () => ctx.drawImage(other, 0, 0, 0, 5, 0, 0, 5, 5))
            ctx.reset()
            let id = ctx.createImageData(3, 3)
            for (let i = 0; i < 36; i += 4) { id.data[i + 1] = 255; id.data[i + 3] = 255 }
            tryIt("drawImage imagedata", () => (ctx.drawImage(id, 50, 50) === ctx) + " " + root.px(ctx, 51, 51) + " " + root.px(ctx, 54, 54))
            ctx.reset()
            ctx.shadowColor = "#ff0000"; ctx.shadowOffsetX = 30
            ctx.drawImage(other, 0, 0)
            console.log("drawImage shadow", px(ctx, 5, 5), px(ctx, 35, 5))
            ctx.reset()
            ctx.globalAlpha = 0.5; ctx.drawImage(other, 0, 0)
            console.log("drawImage alpha", px(ctx, 5, 15))
            console.log("END")
        }
    }
    Timer { interval: 150; running: true; onTriggered: canvas.requestPaint() }
    Timer { interval: 500; running: true; onTriggered: { console.log("DONE"); Qt.quit() } }
}
