import QtQuick

// Canvas and Context2D against Qt 6.12 (CanvasWeb.test.js): the item's properties, the state as Qt reads and checks it
// (colors, numbers, names, fill rule, fonts, dashes), what the methods return and throw, the pixel array, and the pixels
// of fills, strokes, transforms, gradients, compositions, clips, shadows, curves, dashes, image data, save / restore.
// The expected lines are CanvasState.qt.txt (qml -platform offscreen of Qt 6.12).
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
        id: canvas
        width: 100; height: 80
        onPaint: (region) => {
            if (root.step++ > 0) return
            console.log("region", region.x, region.y, region.width, region.height)
            console.log("item", available, canvasSize.width, canvasSize.height, canvasWindow.width, tileSize.width, renderTarget, renderStrategy, contextType)
            let ctx = getContext("2d")
            console.log("contextType", contextType, getContext("2d") === ctx, ctx.canvas === canvas)
            let props = ["fillStyle", "strokeStyle", "globalAlpha", "globalCompositeOperation", "lineWidth", "lineCap", "lineJoin", "miterLimit", "lineDashOffset", "shadowOffsetX", "shadowOffsetY", "shadowBlur", "shadowColor", "font", "textAlign", "textBaseline", "fillRule"]
            for (let p of props) console.log("default", p, typeof ctx[p], ctx[p])
            console.log("lineDash", JSON.stringify(ctx.getLineDash()))
            // colors
            for (let c of ["red", "#80ff0000", "#f00", "rgba(1, 2, 3, 0.5)", "rgb(10,20,30)", "hsl(120, 100%, 50%)", "transparent", "foo", "#12345", ""]) {
                ctx.fillStyle = "#000000"
                ctx.fillStyle = c
                ctx.strokeStyle = c
                ctx.shadowColor = c
                console.log("color", JSON.stringify(c), ctx.fillStyle, ctx.strokeStyle, ctx.shadowColor)
            }
            ctx.fillStyle = Qt.rgba(0, 0.5, 1, 0.25)
            console.log("color qt.rgba", ctx.fillStyle)
            ctx.fillStyle = "steelblue"
            console.log("color name", ctx.fillStyle)
            // numbers
            for (let p of ["lineWidth", "miterLimit", "globalAlpha", "shadowBlur", "shadowOffsetX", "lineDashOffset"]) {
                let out = []
                for (let v of [3, 0, -1, NaN, Infinity, 0.5, "2", 2.5]) {
                    ctx[p] = v
                    out.push(ctx[p])
                }
                console.log("number", p, out.join(" "))
                ctx.reset()
            }
            // enums
            for (let p of ["lineCap", "lineJoin", "textAlign", "textBaseline", "globalCompositeOperation"]) {
                let out = []
                for (let v of ["round", "square", "butt", "bevel", "miter", "start", "end", "left", "right", "center", "top", "hanging", "middle", "alphabetic", "ideographic", "bottom", "copy", "xor", "lighter", "qt-multiply", "multiply", "qt-exclusion", "darker", "bogus"]) {
                    ctx[p] = v
                    out.push(ctx[p])
                }
                console.log("enum", p, out.join(" "))
                ctx.reset()
            }
            for (let v of [Qt.OddEvenFill, Qt.WindingFill, "OddEvenFill", "WindingFill", "bogus", 7]) { ctx.fillRule = v; console.log("fillRule", JSON.stringify(v), ctx.fillRule) }
            ctx.reset()
            for (let f of ["bold 20px Arial", "italic 12pt serif", "small-caps 14px monospace", "normal normal 600 16px 'Times New Roman'", "20px", "bogus", "oblique bold 9px sans-serif", "bold italic 30px Arial, Helvetica"]) {
                ctx.font = "10px sans-serif"
                ctx.font = f
                console.log("font", JSON.stringify(f), JSON.stringify(ctx.font))
            }
            ctx.reset()
            ctx.setLineDash([5, 3, 2]); console.log("dash", JSON.stringify(ctx.getLineDash()))
            ctx.setLineDash([1, -2]); console.log("dash neg", JSON.stringify(ctx.getLineDash()))
            ctx.setLineDash([4, NaN]); console.log("dash nan", JSON.stringify(ctx.getLineDash()))
            ctx.setLineDash([]); console.log("dash empty", JSON.stringify(ctx.getLineDash()))
            // return values
            let rets = []
            let calls = {
                save: () => ctx.save(), restore: () => ctx.restore(), beginPath: () => ctx.beginPath(), closePath: () => ctx.closePath(),
                moveTo: () => ctx.moveTo(1, 1), lineTo: () => ctx.lineTo(2, 2), rect: () => ctx.rect(0, 0, 1, 1), arc: () => ctx.arc(5, 5, 2, 0, 1),
                arcTo: () => ctx.arcTo(0, 0, 5, 5, 1), quadraticCurveTo: () => ctx.quadraticCurveTo(1, 1, 2, 2), bezierCurveTo: () => ctx.bezierCurveTo(1, 1, 2, 2, 3, 3),
                ellipse: () => ctx.ellipse(0, 0, 4, 4), roundedRect: () => ctx.roundedRect(0, 0, 4, 4, 1, 1), text: () => ctx.text("a", 0, 0),
                fill: () => ctx.fill(), stroke: () => ctx.stroke(), clip: () => ctx.clip(), fillRect: () => ctx.fillRect(0, 0, 0, 0),
                strokeRect: () => ctx.strokeRect(0, 0, 0, 0), clearRect: () => ctx.clearRect(0, 0, 0, 0), fillText: () => ctx.fillText("", 0, 0),
                strokeText: () => ctx.strokeText("", 0, 0), rotate: () => ctx.rotate(0), scale: () => ctx.scale(1, 1), translate: () => ctx.translate(0, 0),
                transform: () => ctx.transform(1, 0, 0, 1, 0, 0), setTransform: () => ctx.setTransform(1, 0, 0, 1, 0, 0), resetTransform: () => ctx.resetTransform(),
                shear: () => ctx.shear(0, 0), reset: () => ctx.reset(), setLineDash: () => ctx.setLineDash([]), drawImage: () => ctx.drawImage("nothing.png", 0, 0),
                putImageData: () => ctx.putImageData(ctx.createImageData(1, 1), 0, 0), isPointInPath: () => ctx.isPointInPath(0, 0),
                measureText: () => ctx.measureText("x"), createLinearGradient: () => ctx.createLinearGradient(0, 0, 1, 1),
                createRadialGradient: () => ctx.createRadialGradient(0, 0, 1, 0, 0, 2), createConicalGradient: () => ctx.createConicalGradient(0, 0, 0),
                createPattern: () => ctx.createPattern("red", Qt.SolidPattern), createImageData: () => ctx.createImageData(2, 3), getImageData: () => ctx.getImageData(0, 0, 2, 2),
            }
            for (let name in calls) {
                let v
                try { v = calls[name]() } catch (e) { v = "throws " + e.code + " " + e.message }
                rets.push(name + "=" + (v === ctx ? "ctx" : v === undefined ? "undefined" : v === null ? "null" : typeof v === "object" ? "object" : typeof v + ":" + v))
            }
            console.log("returns", rets.join(" "))
            ctx.reset()
            // arguments
            tryIt("arc neg", () => ctx.arc(0, 0, -1, 0, 1) === ctx)
            tryIt("arcTo neg", () => ctx.arcTo(0, 0, 1, 1, -1) === ctx)
            tryIt("arc few", () => ctx.arc(0, 0) === ctx)
            tryIt("fillRect few", () => ctx.fillRect(0, 0) === ctx)
            tryIt("addColorStop range", () => ctx.createLinearGradient(0, 0, 1, 1).addColorStop(2, "red"))
            tryIt("addColorStop color", () => ctx.createLinearGradient(0, 0, 1, 1).addColorStop(0.5, "foo"))
            tryIt("linear few", () => typeof ctx.createLinearGradient(0, 0))
            tryIt("linear nan", () => typeof ctx.createLinearGradient(0, 0, NaN, 1))
            tryIt("radial neg", () => typeof ctx.createRadialGradient(0, 0, -1, 0, 0, 1))
            tryIt("conical nan", () => typeof ctx.createConicalGradient(NaN, 0, 0))
            tryIt("conical angle nan", () => typeof ctx.createConicalGradient(0, 0, NaN))
            tryIt("imageData zero", () => ctx.createImageData(0, 1))
            tryIt("imageData neg", () => { let d = ctx.createImageData(-2, 3); return d.width + "x" + d.height })
            tryIt("imageData few", () => ctx.createImageData())
            tryIt("getImageData zero", () => ctx.getImageData(0, 0, 0, 1))
            tryIt("getImageData neg", () => { let d = ctx.getImageData(10, 10, -2, -3); return d.width + "x" + d.height })
            tryIt("getImageData few", () => ctx.getImageData(0, 0))
            tryIt("putImageData bad", () => ctx.putImageData({}, 0, 0))
            tryIt("putImageData few", () => ctx.putImageData(ctx.createImageData(1, 1)))
            tryIt("drawFocusRing", () => ctx.drawFocusRing())
            tryIt("caretBlinkRate", () => ctx.caretBlinkRate())
            tryIt("measureText", () => { let m = ctx.measureText("Hello"); return typeof m.width + " " + Object.keys(m).join(",") })
            tryIt("measureText none", () => ctx.measureText().width)
            tryIt("imageData props", () => { let d = ctx.createImageData(2, 3); return d.width + " " + d.height + " " + d.data.length + " " + d.data[0] + " " + typeof d.data })
            tryIt("pixel array write", () => { let d = ctx.createImageData(1, 1); d.data[0] = 300; d.data[1] = -5; d.data[2] = 12.7; d.data[3] = "40"; return d.data[0] + " " + d.data[1] + " " + d.data[2] + " " + d.data[3] + " " + d.data[9] })
            tryIt("imageData copy", () => { let a = ctx.createImageData(2, 2); a.data[0] = 9; let b = ctx.createImageData(a); return b.width + " " + b.data[0] })
            tryIt("getContext other", () => getContext("webgl"))
            tryIt("getContext case", () => getContext("2D") === ctx)
            ctx.reset()
            // pixels
            ctx.fillStyle = "red"
            ctx.fillRect(10, 10, 20, 20)
            console.log("px fillRect", px(ctx, 15, 15), px(ctx, 5, 5))
            ctx.globalAlpha = 0.5
            ctx.fillStyle = "#0000ff"
            ctx.fillRect(40, 10, 10, 10)
            console.log("px alpha", px(ctx, 45, 15))
            ctx.globalAlpha = 1
            ctx.fillStyle = "#8000ff00"
            ctx.fillRect(60, 10, 10, 10)
            console.log("px qt argb", px(ctx, 65, 15))
            ctx.reset()
            ctx.strokeStyle = "black"; ctx.lineWidth = 4
            ctx.beginPath(); ctx.moveTo(10, 40); ctx.lineTo(90, 40); ctx.stroke()
            console.log("px stroke", px(ctx, 50, 40), px(ctx, 50, 41), px(ctx, 50, 43))
            ctx.reset()
            ctx.fillStyle = "black"
            ctx.beginPath(); ctx.rect(10, 10, 40, 40); ctx.rect(20, 20, 20, 20); ctx.fill()
            console.log("px winding", px(ctx, 30, 30))
            ctx.clearRect(0, 0, 100, 80)
            ctx.fillRule = Qt.OddEvenFill
            ctx.beginPath(); ctx.rect(10, 10, 40, 40); ctx.rect(20, 20, 20, 20); ctx.fill()
            console.log("px oddeven", px(ctx, 30, 30), px(ctx, 15, 15))
            console.log("inPath", ctx.isPointInPath(30, 30), ctx.isPointInPath(15, 15), ctx.isPointInPath(5, 5))
            ctx.reset()
            ctx.translate(50, 40); ctx.rotate(Math.PI / 2); ctx.fillStyle = "red"; ctx.fillRect(0, 0, 10, 5)
            console.log("px rotate", px(ctx, 47, 45), px(ctx, 52, 45))
            ctx.reset()
            ctx.scale(2, 2); ctx.shear(0.5, 0); ctx.fillStyle = "blue"; ctx.fillRect(10, 10, 5, 5)
            console.log("px shear", px(ctx, 32, 22), px(ctx, 21, 21))
            ctx.reset()
            let g = ctx.createLinearGradient(0, 0, 100, 0)
            g.addColorStop(0, "#ff0000"); g.addColorStop(1, "#0000ff")
            ctx.fillStyle = g; ctx.fillRect(0, 0, 100, 10)
            console.log("px linear", px(ctx, 1, 5), px(ctx, 50, 5), px(ctx, 98, 5))
            ctx.reset()
            let cg = ctx.createConicalGradient(50, 40, 0)
            cg.addColorStop(0, "#000000"); cg.addColorStop(1, "#ffffff")
            ctx.fillStyle = cg; ctx.fillRect(0, 0, 100, 80)
            console.log("px conical", px(ctx, 50, 20), px(ctx, 50, 60), px(ctx, 20, 40), px(ctx, 80, 41))
            ctx.reset()
            let rg = ctx.createRadialGradient(50, 40, 0, 50, 40, 40)
            rg.addColorStop(0, "white"); rg.addColorStop(1, "black")
            ctx.fillStyle = rg; ctx.fillRect(0, 0, 100, 80)
            console.log("px radial", px(ctx, 50, 40), px(ctx, 70, 40), px(ctx, 95, 75))
            ctx.reset()
            ctx.fillStyle = "red"; ctx.fillRect(0, 0, 50, 50)
            ctx.globalCompositeOperation = "qt-multiply"
            ctx.fillStyle = "#00ff00"; ctx.fillRect(25, 25, 50, 50)
            console.log("px multiply", px(ctx, 30, 30), px(ctx, 60, 60), ctx.globalCompositeOperation)
            ctx.reset()
            ctx.fillStyle = "red"; ctx.fillRect(0, 0, 50, 50)
            ctx.globalCompositeOperation = "destination-out"
            ctx.fillRect(25, 25, 50, 50)
            console.log("px dest-out", px(ctx, 30, 30), px(ctx, 10, 10))
            ctx.reset()
            ctx.fillStyle = "red"; ctx.fillRect(0, 0, 50, 50)
            ctx.globalCompositeOperation = "copy"
            ctx.fillStyle = "blue"; ctx.fillRect(40, 40, 20, 20)
            console.log("px copy", px(ctx, 10, 10), px(ctx, 45, 45))
            ctx.reset()
            ctx.beginPath(); ctx.rect(20, 20, 20, 20); ctx.clip()
            ctx.fillStyle = "green"; ctx.fillRect(0, 0, 100, 80)
            console.log("px clip", px(ctx, 30, 30), px(ctx, 10, 10))
            ctx.reset()
            ctx.shadowColor = "black"; ctx.shadowOffsetX = 20; ctx.shadowOffsetY = 0; ctx.shadowBlur = 0
            ctx.fillStyle = "red"; ctx.fillRect(10, 10, 10, 10)
            console.log("px shadow", px(ctx, 15, 15), px(ctx, 35, 15))
            ctx.reset()
            ctx.fillStyle = "black"; ctx.beginPath(); ctx.ellipse(10, 10, 40, 20); ctx.fill()
            console.log("px ellipse", px(ctx, 30, 20), px(ctx, 12, 12), px(ctx, 52, 20))
            ctx.reset()
            ctx.fillStyle = "black"; ctx.beginPath(); ctx.roundedRect(10, 10, 40, 40, 10, 10); ctx.fill()
            console.log("px roundedRect", px(ctx, 11, 11), px(ctx, 30, 30), px(ctx, 20, 11))
            ctx.reset()
            ctx.fillStyle = "black"; ctx.beginPath(); ctx.arc(50, 40, 20, 0, Math.PI, false); ctx.fill()
            console.log("px arc", px(ctx, 50, 50), px(ctx, 50, 30))
            ctx.reset()
            ctx.fillStyle = "black"; ctx.beginPath(); ctx.arc(50, 40, 20, 0, Math.PI, true); ctx.fill()
            console.log("px arc ccw", px(ctx, 50, 50), px(ctx, 50, 30))
            ctx.reset()
            ctx.path = "M 10 10 L 60 10 L 60 60 Z"
            ctx.fillStyle = "black"; ctx.fill()
            console.log("px svgpath", px(ctx, 50, 20), px(ctx, 20, 50), typeof ctx.path)
            ctx.reset()
            ctx.lineWidth = 6; ctx.setLineDash([10, 10]); ctx.strokeStyle = "black"
            ctx.beginPath(); ctx.moveTo(0, 20); ctx.lineTo(100, 20); ctx.stroke()
            console.log("px dash", px(ctx, 5, 20), px(ctx, 15, 20), px(ctx, 25, 20), px(ctx, 35, 20))
            ctx.reset()
            ctx.lineWidth = 6; ctx.setLineDash([10, 10]); ctx.lineDashOffset = 5; ctx.strokeStyle = "black"
            ctx.beginPath(); ctx.moveTo(0, 20); ctx.lineTo(100, 20); ctx.stroke()
            console.log("px dash offset", px(ctx, 2, 20), px(ctx, 7, 20), px(ctx, 12, 20), px(ctx, 17, 20))
            ctx.reset()
            ctx.lineWidth = 10; ctx.lineCap = "square"; ctx.strokeStyle = "black"
            ctx.beginPath(); ctx.moveTo(20, 20); ctx.lineTo(40, 20); ctx.stroke()
            console.log("px cap square", px(ctx, 17, 20), px(ctx, 43, 20))
            ctx.reset()
            ctx.fillStyle = "red"; ctx.fillRect(0, 0, 10, 10)
            let id = ctx.getImageData(0, 0, 10, 10)
            ctx.putImageData(id, 20, 20)
            console.log("px putImageData", px(ctx, 25, 25), id.width, id.height, id.data.length)
            let id2 = ctx.createImageData(4, 4)
            for (let i = 0; i < id2.data.length; i += 4) { id2.data[i] = 0; id2.data[i + 1] = 0; id2.data[i + 2] = 255; id2.data[i + 3] = 128 }
            ctx.putImageData(id2, 50, 50)
            console.log("px put alpha", px(ctx, 51, 51))
            ctx.reset()
            ctx.fillStyle = "red"; ctx.fillRect(0, 0, 100, 80)
            ctx.save(); ctx.fillStyle = "blue"; ctx.lineWidth = 7; ctx.translate(10, 10); ctx.save(); ctx.fillStyle = "green"; ctx.restore()
            console.log("state", ctx.fillStyle, ctx.lineWidth)
            ctx.restore(); ctx.restore(); ctx.restore()
            console.log("state restored", ctx.fillStyle, ctx.lineWidth)
            ctx.reset()
            console.log("after reset", px(ctx, 5, 5), ctx.fillStyle)
            ctx.fillStyle = "red"; ctx.fillRect(0, 0, 10, 10)
            let url = canvas.toDataURL()
            console.log("toDataURL", url.slice(0, 22), canvas.toDataURL("image/bogus"), canvas.toDataURL("text/plain"), canvas.toDataURL("image/jpeg").slice(0, 23))
            console.log("END")
        }
        onPainted: console.log("painted")
    }
    Timer { interval: 300; running: true; onTriggered: { console.log("DONE"); Qt.quit() } }
}
