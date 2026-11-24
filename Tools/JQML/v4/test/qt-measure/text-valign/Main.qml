// Text.verticalAlignment in desktop Qt: where the first and the last row of ink are for a text in boxes that are
// higher, equal and lower than the text, with padding and with several lines.
// QT_QPA_PLATFORM=offscreen QT_FORCE_STDERR_LOGGING=1 qml.exe Main.qml
import QtQuick
import QtTest

Item {
    id: root
    width: 300
    height: 300

    property var cases: [
        { n: "top-high", h: 60, a: Text.AlignTop },
        { n: "center-high", h: 60, a: Text.AlignVCenter },
        { n: "bottom-high", h: 60, a: Text.AlignBottom },
        { n: "default-high", h: 60, a: -1 },
        { n: "center-equal", h: 20, a: Text.AlignVCenter },
        { n: "center-low", h: 8, a: Text.AlignVCenter },
        { n: "bottom-low", h: 8, a: Text.AlignBottom },
        { n: "center-pad", h: 60, a: Text.AlignVCenter, pt: 10, pb: 2 },
        { n: "bottom-pad", h: 60, a: Text.AlignBottom, pt: 10, pb: 2 },
        { n: "center-two-lines", h: 60, a: Text.AlignVCenter, t: "AAAA\nAAAA" },
        { n: "center-elide", h: 60, a: Text.AlignVCenter, elide: Text.ElideMiddle, w: 40 },
        { n: "center-rich", h: 60, a: Text.AlignVCenter, rich: true },
        { n: "center-wrap-low", h: 20, a: Text.AlignVCenter, wrap: true, w: 30, t: "AAAA AAAA AAAA" }
    ]
    property int index: 0
    Component.onCompleted: console.log("loaded")

    Text {
        id: probe
        color: "black"
        font.pixelSize: 16
        font.family: "Arial"
    }

    function fill(c) {
        probe.text = c.t === undefined ? "AAAA" : c.t
        probe.textFormat = c.rich ? Text.RichText : Text.PlainText
        probe.width = c.w === undefined ? 200 : c.w
        probe.height = c.h
        probe.elide = c.elide === undefined ? Text.ElideNone : c.elide
        probe.wrapMode = c.wrap ? Text.WordWrap : Text.NoWrap
        probe.topPadding = c.pt === undefined ? 0 : c.pt
        probe.bottomPadding = c.pb === undefined ? 0 : c.pb
        if (c.a >= 0)
            probe.verticalAlignment = c.a
        else
            probe.verticalAlignment = Text.AlignTop
    }

    TestCase { id: tc; name: "valign"; when: windowShown }

    Timer {
        interval: 600
        running: true
        onTriggered: root.run()
    }

    function run() {
        for (let i = 0; i < cases.length; i++) {
            const c = cases[i]
            fill(c)
            tc.wait(80)
            const image = tc.grabImage(probe)
            let first = -1, last = -1
            for (let y = 0; y < image.height; y++) {
                for (let x = 0; x < image.width; x++) {
                    const p = image.pixel(x, y)
                    if (p.r < 0.5) {
                        if (first < 0) first = y
                        last = y
                        break
                    }
                }
            }
            console.log("valign", c.n, "| size", probe.width, probe.height, "content", probe.contentWidth, probe.contentHeight, "ink", first, last)
        }
        Qt.quit()
    }
}
