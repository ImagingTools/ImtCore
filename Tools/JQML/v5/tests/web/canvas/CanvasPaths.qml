import QtQuick

// Canvas and Context2D against Qt 6.12 (CanvasWeb.test.js): the order of transform(), a matrix that cannot be inverted,
// isPointInPath on the border, arcTo and arcs past a turn, gradients kept as they were set, Qt's brush patterns and
// textures, putImageData with its dirty rectangle, clips across save / restore and reset, text placement.
// The expected lines are CanvasPaths.qt.txt (qml -platform offscreen of Qt 6.12).
Item {
    id: root
    width: 200; height: 200
    property int step: 0
    property int paintedCount: 0
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
        onPaint: { let c = getContext("2d"); c.fillStyle = "#00ff00"; c.fillRect(0, 0, 20, 20) }
    }
    Canvas {
        id: canvas
        width: 100; height: 80
        onPainted: root.paintedCount++
        onPaint: {
            if (root.step++ > 0) return
            let ctx = getContext("2d")
            // transform order (state.matrix * transform)
            ctx.scale(2, 2); ctx.transform(1, 0, 0, 1, 10, 0); ctx.fillStyle = "red"; ctx.fillRect(0, 0, 5, 5)
            console.log("transform order", px(ctx, 11, 1), px(ctx, 21, 1))
            ctx.reset()
            ctx.translate(10, 10); ctx.setTransform(1, 0, 0, 1, 30, 0); ctx.fillStyle = "blue"; ctx.fillRect(0, 0, 5, 5)
            console.log("setTransform", px(ctx, 31, 1), px(ctx, 41, 11))
            ctx.reset()
            ctx.scale(0, 1); ctx.fillStyle = "red"; ctx.fillRect(0, 0, 10, 10); ctx.translate(5, 5)
            console.log("singular", px(ctx, 5, 5))
            ctx.resetTransform(); ctx.fillRect(0, 0, 10, 10)
            console.log("singular reset", px(ctx, 5, 5))
            ctx.reset()
            ctx.setTransform(1, 0, 0, 1, 0, 0); ctx.scale(0, 1); ctx.setTransform(1, 0, 0, 1, 0, 0); ctx.fillStyle = "red"; ctx.fillRect(0, 0, 10, 10)
            console.log("singular setTransform", px(ctx, 5, 5))
            ctx.reset()
            // path details
            ctx.beginPath(); ctx.moveTo(10, 10); ctx.lineTo(10, 10); ctx.closePath(); ctx.lineTo(20, 10)
            console.log("inPath line", ctx.isPointInPath(15, 10), ctx.isPointInPath(15, 11))
            ctx.beginPath(); ctx.rect(10, 10, 20, 20)
            console.log("inPath border", ctx.isPointInPath(10, 15), ctx.isPointInPath(30, 30), ctx.isPointInPath(31, 30))
            ctx.translate(5, 5)
            console.log("inPath translated", ctx.isPointInPath(12, 12), ctx.isPointInPath(32, 32))
            ctx.reset()
            ctx.beginPath(); ctx.moveTo(10, 50); ctx.arcTo(50, 50, 50, 10, 20); ctx.lineTo(50, 10)
            ctx.lineWidth = 4; ctx.strokeStyle = "black"; ctx.stroke()
            console.log("arcTo", px(ctx, 30, 50), px(ctx, 50, 30), px(ctx, 44, 36), px(ctx, 49, 49))
            ctx.reset()
            ctx.beginPath(); ctx.arc(50, 40, 20, 0, 3 * Math.PI, false); ctx.fillStyle = "black"; ctx.fill()
            console.log("arc 3pi", px(ctx, 50, 30), px(ctx, 50, 50))
            ctx.reset()
            ctx.beginPath(); ctx.arc(50, 40, 20, 0, -7.5, false); ctx.fillStyle = "black"; ctx.fill()
            console.log("arc -7.5", px(ctx, 50, 25), px(ctx, 50, 55), px(ctx, 65, 45), px(ctx, 65, 35))
            ctx.reset()
            ctx.beginPath(); ctx.moveTo(10, 40); ctx.arc(50, 40, 0, 0, 1); ctx.lineWidth = 2; ctx.stroke()
            console.log("arc r0", px(ctx, 30, 40), px(ctx, 52, 40))
            ctx.reset()
            // styles
            let g = ctx.createLinearGradient(0, 0, 100, 0)
            g.addColorStop(0, "red")
            ctx.fillStyle = g
            g.addColorStop(1, "blue")
            console.log("gradient getter", ctx.fillStyle === g, typeof ctx.fillStyle)
            ctx.fillRect(0, 0, 100, 10)
            console.log("gradient snapshot", px(ctx, 90, 5))
            ctx.strokeStyle = {}
            console.log("stroke object", typeof ctx.strokeStyle, JSON.stringify(ctx.strokeStyle))
            ctx.strokeStyle = "green"
            let g2 = ctx.createLinearGradient(0, 0, 10, 0)
            console.log("addColorStop returns", g2.addColorStop(0.5, "red") === g2, g2.addColorStop(1) === g2)
            ctx.reset()
            let p = ctx.createPattern("#ff0000", Qt.Dense4Pattern)
            console.log("pattern getter", (ctx.fillStyle = p, ctx.fillStyle))
            ctx.fillRect(0, 0, 16, 16)
            console.log("pattern dense4", px(ctx, 0, 0), px(ctx, 1, 0), px(ctx, 0, 1), px(ctx, 1, 1))
            ctx.reset()
            ctx.fillStyle = ctx.createPattern("#0000ff", Qt.HorPattern); ctx.fillRect(0, 0, 16, 16)
            console.log("pattern hor", px(ctx, 0, 3), px(ctx, 0, 2), px(ctx, 0, 11))
            ctx.reset()
            ctx.fillStyle = ctx.createPattern("#0000ff", Qt.NoBrush); ctx.fillRect(0, 0, 16, 16)
            console.log("pattern none", px(ctx, 5, 5), ctx.fillStyle)
            ctx.reset()
            let id = ctx.createImageData(2, 2)
            id.data[0] = 255; id.data[3] = 255; id.data[6] = 255; id.data[7] = 255
            ctx.fillStyle = ctx.createPattern(id, "no-repeat"); ctx.fillRect(0, 0, 8, 8)
            console.log("pattern texture", px(ctx, 0, 0), px(ctx, 1, 0), px(ctx, 2, 0), px(ctx, 5, 5))
            ctx.reset()
            tryIt("pattern few", () => ctx.createPattern("red"))
            tryIt("pattern item", () => { ctx.fillStyle = ctx.createPattern(other, "repeat"); ctx.fillRect(0, 0, 4, 4); return root.px(ctx, 1, 1) + " " + ctx.fillStyle })
            ctx.reset()
            // images: drawImage of another canvas (CanvasImages.qml)
            let src = ctx.createImageData(4, 4)
            for (let i = 0; i < 64; i += 4) { src.data[i] = 255; src.data[i + 3] = 255 }
            tryIt("put 7", () => ctx.putImageData(src, 10, 10, 1, 1, 2, 2) === ctx)
            console.log("put 7 px", px(ctx, 10, 10), px(ctx, 11, 11), px(ctx, 12, 12), px(ctx, 13, 13))
            tryIt("put 7 neg", () => ctx.putImageData(src, 20, 20, 3, 3, -2, -2) === ctx)
            console.log("put 7 neg px", px(ctx, 21, 21), px(ctx, 22, 22), px(ctx, 23, 23))
            tryIt("put 7 empty", () => ctx.putImageData(src, 20, 20, 5, 5, 2, 2))
            tryIt("put not object", () => ctx.putImageData(5, 0, 0, 0, 0, 1, 1))
            tryIt("put nan", () => ctx.putImageData(src, NaN, 0, 0, 0, 1, 1))
            ctx.reset()
            ctx.globalAlpha = 0.5; ctx.putImageData(src, 0, 0, 0, 0, 4, 4)
            console.log("put alpha state", px(ctx, 1, 1))
            ctx.reset()
            ctx.translate(30, 0); ctx.putImageData(src, 0, 0, 0, 0, 4, 4)
            console.log("put transform", px(ctx, 1, 1), px(ctx, 31, 1))
            ctx.reset()
            ctx.fillStyle = "red"; ctx.fillRect(10, 20, 4, 4)
            let gd = ctx.getImageData(8, 18, 8, 8)
            console.log("getImageData region", gd.width, gd.height, gd.data[(3 * 8 + 3) * 4], gd.data[0], gd.data.length)
            tryIt("getImageData frac", () => { let d = ctx.getImageData(0.4, 0.6, 2.5, 2.4); return d.width + "x" + d.height })
            ctx.reset()
            // clip and state
            ctx.save(); ctx.beginPath(); ctx.rect(0, 0, 50, 80); ctx.clip()
            ctx.beginPath(); ctx.rect(25, 0, 50, 80); ctx.clip()
            ctx.fillStyle = "red"; ctx.fillRect(0, 0, 100, 80)
            console.log("clip intersect", px(ctx, 10, 5), px(ctx, 30, 5), px(ctx, 60, 5))
            ctx.restore(); ctx.fillStyle = "blue"; ctx.fillRect(0, 50, 100, 30)
            console.log("clip restored", px(ctx, 10, 60), px(ctx, 60, 60))
            ctx.beginPath(); ctx.rect(0, 0, 10, 10); ctx.clip(); ctx.reset(); ctx.fillStyle = "green"; ctx.fillRect(0, 0, 100, 80)
            console.log("clip reset", px(ctx, 50, 50))
            ctx.reset()
            ctx.save(); ctx.translate(10, 10); ctx.beginPath(); ctx.moveTo(0, 0); ctx.lineTo(20, 0); ctx.lineTo(20, 20); ctx.restore()
            ctx.lineTo(0, 20); ctx.fillStyle = "black"; ctx.fill()
            console.log("path across restore", px(ctx, 25, 15), px(ctx, 5, 15), px(ctx, 20, 25))
            ctx.reset()
            // text
            ctx.font = "bold 20px sans-serif"
            let w = ctx.measureText("Hello").width
            console.log("measure int", w === Math.round(w), w > 40 && w < 60)
            ctx.fillStyle = "black"
            ctx.textAlign = "center"; ctx.textBaseline = "top"; ctx.fillText("MMMM", 50, 10)
            let dd = ctx.getImageData(0, 0, 100, 80).data
            let minx = 100, maxx = -1, miny = 80, maxy = -1
            for (let y = 0; y < 80; y++) for (let x = 0; x < 100; x++) if (dd[(y * 100 + x) * 4 + 3] > 128) { minx = Math.min(minx, x); maxx = Math.max(maxx, x); miny = Math.min(miny, y); maxy = Math.max(maxy, y) }
            console.log("text center top", Math.abs((minx + maxx) / 2 - 50) <= 2, miny >= 10 && miny <= 16, maxy < 34)
            ctx.reset()
            ctx.font = "20px sans-serif"; ctx.fillStyle = "black"; ctx.text("I", 10, 30); ctx.fill()
            console.log("text path", px(ctx, 12, 20) !== "0,0,0,0", px(ctx, 12, 35))
            ctx.reset()
            console.log("textBaseline/Align after reset", ctx.textAlign, ctx.textBaseline, ctx.font)
            console.log("painted so far", root.paintedCount > 0)
            console.log("END")
        }
    }
    Timer { interval: 400; running: true; onTriggered: { console.log("painted total", root.paintedCount > 1); console.log("DONE"); Qt.quit() } }
}
