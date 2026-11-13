import QtQuick

// createImageData(width, height) / createImageData(imageData): the size (rounded), what is refused, and what the pixel array holds.
// putImageData() draws the pixels of the image; with no rectangle of the pixels to take it draws nothing (Qt).
Item {
    width: 40
    height: 40

    Canvas {
        id: canvas
        width: 40
        height: 40
        onPaint: {
            var ctx = getContext("2d")
            var a = ctx.createImageData(3, 2)
            console.log("created", typeof a, a.width, a.height, a.data.length, a.data[0], a.data[23], typeof a.data[24])
            var b = ctx.createImageData(a)
            console.log("copy", b.width, b.height, b.data.length, b !== a)
            var tries = [[0, 0], [0, 3], [-2, 3], [3, -2], [2.7, 1.2], ["3", "2"], [undefined, 2]]
            for (var i = 0; i < tries.length; i++) {
                try {
                    var r = ctx.createImageData(tries[i][0], tries[i][1])
                    console.log("size", JSON.stringify(tries[i]), r.width, r.height, r.data.length)
                } catch (e) {
                    console.log("size", JSON.stringify(tries[i]), "refused:", e.message)
                }
            }
            console.log("no arguments", ctx.createImageData())
            console.log("an object", ctx.createImageData({ width: 2, height: 2 }))
            a.data[2] = 12
            a.data[3] = 255
            console.log("written", a.data[2], a.data[3])
            console.log("put returns the context", ctx.putImageData(a, 1, 1, 0, 0, 3, 2) === ctx)
            console.log("put without a rectangle", ctx.putImageData(a, 1, 1))
            Qt.quit()
        }
        Component.onCompleted: requestPaint()
    }
}
