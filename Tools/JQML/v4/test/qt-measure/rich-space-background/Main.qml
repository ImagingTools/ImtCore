// Where the background-color of a span shows in desktop Qt RichText when the span holds spaces (the block cursor of the terminal
// of Agentino is a span with a background that holds one space at the end of the line).
// QT_QPA_FONTDIR=C:/Windows/Fonts QT_QPA_PLATFORM=offscreen QT_FORCE_STDERR_LOGGING=1 qml.exe Main.qml
import QtQuick
import QtTest

Item {
    id: root
    width: 300
    height: 100

    property string bg: "style=\"background-color:#0000ff\""
    property var cases: [
        { n: "word, space at the end", t: "AB<span " + bg + "> </span>" },
        { n: "word, two spaces at the end", t: "AB<span " + bg + ">  </span>" },
        { n: "word, space in the middle", t: "A<span " + bg + "> </span>B" },
        { n: "span over a word with a space", t: "<span " + bg + ">A B</span>" },
        { n: "span over a word and a space", t: "<span " + bg + ">AB </span>" },
        { n: "space at the start", t: "<span " + bg + "> </span>AB" },
        { n: "only a space", t: "<span " + bg + "> </span>" },
        { n: "nbsp at the end", t: "AB<span " + bg + ">&nbsp;</span>" },
        { n: "space at the end, white-space pre", t: "<span style=\"white-space:pre\">AB<span " + bg + "> </span></span>" },
        { n: "space after a coloured span", t: "<span style=\"color:#ff0000\">AB</span><span " + bg + ";color:#ffffff\"> </span>" },
        { n: "plain text with a trailing space", t: "AB " },
        { n: "rich: AB then a space", t: "AB " },
        { n: "rich: AB then two spaces", t: "AB  " },
        { n: "rich: bold AB then a space", t: "<b>AB</b> " },
        { n: "rich: AB then a bold space", t: "AB<b> </b>" },
        { n: "rich: span AB then a plain space", t: "<span " + bg + ">AB</span> " },
        { n: "rich: A, a space, a span and a space", t: "A <span " + bg + ">B</span> " },
        { n: "rich: two lines, space at the end of the first", t: "AB<span " + bg + "> </span><br/>CD" },
        { n: "rich: a space then a break", t: "AB <br/>CD" },
        { n: "rich: p with a trailing space", t: "<p>AB </p>" },
        { n: "rich: space before a p", t: "AB <p>CD</p>" },
        { n: "rich: p, space, longer second p", t: "<p>AB </p><p>CDEF</p>" },
        { n: "rich: bg space at the end of a p", t: "<p>AB<span " + bg + "> </span></p>" },
        { n: "rich: bg space at the end of the first p", t: "<p>AB<span " + bg + "> </span></p><p>CD</p>" },
        { n: "rich: div with a trailing space", t: "<div>AB </div>" },
        { n: "rich: pre with a trailing space", t: "<pre>AB </pre>" },
        { n: "rich: nested bold space", t: "<b>AB<i> </i></b>" }
    ]

    Text {
        id: probe
        font.pixelSize: 16
        font.family: "Arial"
        textFormat: Text.RichText
        color: "black"
    }

    TestCase { id: tc; name: "bg"; when: windowShown }

    Timer {
        interval: 600
        running: true
        onTriggered: root.run()
    }

    function run() {
        for (let i = 0; i < cases.length; i++) {
            const c = cases[i]
            probe.textFormat = c.n.indexOf("plain") === 0 ? Text.PlainText : Text.RichText
            probe.text = c.t
            tc.wait(80)
            const image = tc.grabImage(probe)
            let first = -1, last = -1, count = 0
            for (let x = 0; x < image.width; x++) {
                for (let y = 0; y < image.height; y++) {
                    const p = image.pixel(x, y)
                    if (p.b > 0.8 && p.r < 0.2 && p.g < 0.2) {
                        if (first < 0) first = x
                        last = x
                        count++
                        break
                    }
                }
            }
            console.log("bg", c.n, "| content", probe.contentWidth, probe.contentHeight, "blue columns", first, last, count)
        }
        Qt.quit()
    }
}
