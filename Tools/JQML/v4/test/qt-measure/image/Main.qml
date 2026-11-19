import QtQuick

// Image.sourceSize with a loaded picture: pic.png is 40 x 20, tall.png 20 x 40, pic.svg 40 x 20 (a vector). Every line is
// name, status, sourceSize, implicit size, size, painted size.
Item {
    id: root
    width: 200
    height: 200

    property var cases: [
        { n: "png plain", s: "pic.png" },
        { n: "png 20x10", s: "pic.png", w: 20, h: 10 },
        { n: "png w10", s: "pic.png", w: 10 },
        { n: "png h10", s: "pic.png", h: 10 },
        { n: "png 20x40 (ratio changed)", s: "pic.png", w: 20, h: 40 },
        { n: "png 100x10", s: "pic.png", w: 100, h: 10 },
        { n: "png 80x60 (bigger)", s: "pic.png", w: 80, h: 60 },
        { n: "png w80 (bigger)", s: "pic.png", w: 80 },
        { n: "png h60 (bigger)", s: "pic.png", h: 60 },
        { n: "png 30x30", s: "pic.png", w: 30, h: 30 },
        { n: "png 0x0", s: "pic.png", w: 0, h: 0 },
        { n: "png 20x10 fit", s: "pic.png", w: 20, h: 10, f: Image.PreserveAspectFit },
        { n: "png 30x30 fit", s: "pic.png", w: 30, h: 30, f: Image.PreserveAspectFit },
        { n: "png 30x30 crop", s: "pic.png", w: 30, h: 30, f: Image.PreserveAspectCrop },
        { n: "png w10 fit", s: "pic.png", w: 10, f: Image.PreserveAspectFit },
        { n: "tall plain", s: "tall.png" },
        { n: "tall w10", s: "tall.png", w: 10 },
        { n: "tall 30x30", s: "tall.png", w: 30, h: 30 },
        { n: "svg plain", s: "pic.svg" },
        { n: "svg 20x10", s: "pic.svg", w: 20, h: 10 },
        { n: "svg 80x40 (bigger)", s: "pic.svg", w: 80, h: 40 },
        { n: "svg w80", s: "pic.svg", w: 80 },
        { n: "svg h60", s: "pic.svg", h: 60 },
        { n: "svg 30x30", s: "pic.svg", w: 30, h: 30 },
        { n: "svg 30x30 fit", s: "pic.svg", w: 30, h: 30, f: Image.PreserveAspectFit },
        { n: "svg 100x100 fit", s: "pic.svg", w: 100, h: 100, f: Image.PreserveAspectFit },
        { n: "svg width 50 only", s: "pic.svg", ew: 50 },
        { n: "svg width50 sourceSize 20x10", s: "pic.svg", w: 20, h: 10, ew: 50 },
        { n: "png explicit 100x100 + ss 20x10", s: "pic.png", w: 20, h: 10, ew: 100, eh: 100 },
        { n: "svg explicit 100x100 + ss 20x10 fit", s: "pic.svg", w: 20, h: 10, ew: 100, eh: 100, f: Image.PreserveAspectFit }
    ]
    property var made: []

    Component.onCompleted: {
        var src = "import QtQuick; Image {}"
        for (var i = 0; i < cases.length; i++) {
            var c = cases[i]
            var im = Qt.createQmlObject(src, root)
            if (c.w !== undefined) im.sourceSize.width = c.w
            if (c.h !== undefined) im.sourceSize.height = c.h
            if (c.f !== undefined) im.fillMode = c.f
            if (c.ew !== undefined) im.width = c.ew
            if (c.eh !== undefined) im.height = c.eh
            im.source = c.s
            made.push(im)
        }
    }
    Timer {
        interval: 500
        running: true
        onTriggered: {
            for (var i = 0; i < root.made.length; i++) {
                var im = root.made[i]
                console.log(root.cases[i].n, "|", "status", im.status, "ss", im.sourceSize.width, im.sourceSize.height, "implicit", im.implicitWidth, im.implicitHeight, "size", im.width, im.height, "painted", im.paintedWidth, im.paintedHeight)
            }
            Qt.quit()
        }
    }
}
