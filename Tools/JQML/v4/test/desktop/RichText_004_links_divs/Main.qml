import QtQuick
Item {
    width: 400
    height: 400
    property var cases: [["div text", "Text.RichText", "<div>abc</div>abc", 300], ["div span", "Text.RichText", "<div>abc</div><span>abc</span>", 300], ["text div text", "Text.RichText", "abc<div>abc</div>abc", 300], ["div div text", "Text.RichText", "<div>abc</div><div>abc</div>abc", 300], ["div b", "Text.RichText", "<div>abc</div><b>abc</b>", 300], ["p text", "Text.RichText", "<p>abc</p>abc", 300], ["li text", "Text.RichText", "<ul><li>abc</li></ul>abc", 300], ["p in div", "Text.RichText", "<div><p>abc</p></div>", 300], ["div in p", "Text.RichText", "<p><div>abc</div></p>", 300], ["div text div", "Text.RichText", "<div>abc</div>abc<div>abc</div>", 300], ["nested div", "Text.RichText", "<div><div>abc</div><div>abc</div></div>", 300], ["span block", "Text.RichText", "<span>abc</span><div>abc</div>", 300], ["li two lines", "Text.RichText", "<ul><li>abc<br>abc</li></ul>", 300], ["li then text in ul", "Text.RichText", "<ul>abc<li>abc</li></ul>", 300], ["p inside li then li", "Text.RichText", "<ul><li>abc</li><li><b>abc</b></li></ul>", 300], ["ol text", "Text.RichText", "<ol><li>abc</li></ol>abc", 300], ["link at", "Text.RichText", "abc <a href=\"u\">abc</a> abc", 300]]
    Text { id: t; font.pixelSize: 20; wrapMode: Text.WordWrap }
    Timer {
        interval: 50; running: true
        onTriggered: {
            for (var i = 0; i < cases.length; i++) {
                var f = cases[i][1]
                t.width = cases[i][3]
                t.textFormat = f === "Text.RichText" ? Text.RichText : f === "Text.StyledText" ? Text.StyledText : f === "Text.AutoText" ? Text.AutoText : f === "Text.MarkdownText" ? Text.MarkdownText : Text.PlainText
                t.text = cases[i][2]
                console.log(cases[i][0] + " | cw " + t.contentWidth.toFixed(2) + " ch " + t.contentHeight.toFixed(2) + " iw " + t.implicitWidth.toFixed(2) + " ih " + t.implicitHeight.toFixed(2) + " lines " + t.lineCount)

                if (cases[i][0] === "link at") { console.log("linkAt " + t.linkAt(5, 5) + "|" + t.linkAt(85, 5) + "|" + t.linkAt(150, 5) + "|" + t.linkAt(85, 40) + " hovered [" + t.hoveredLink + "]") }

            }
            Qt.quit()
        }
    }
}
