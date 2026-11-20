// Qt.lighter / darker / alpha / tint / hsla / hsva / rgba over many colors and factors: the 16-bit components and the
// float arithmetic of QColor give these exact names
import QtQuick
Item {
    Component.onCompleted: {
        var colors = ["red", "#336699", "#000000", "#ffffff", "#808080", "#123456", "#fedcba", "#7f7f7f", "#00ff7f",
                      "#80ff0000", "#01020304", "steelblue", "#abc", "#0a0b0c", "#ff8800", "#c0ffee"]
        var factors = [0.1, 0.5, 0.75, 1.0, 1.1, 1.25, 1.5, 2.0, 3.0, 5.0]
        for (var i = 0; i < colors.length; ++i) {
            var line = colors[i] + ":"
            for (var j = 0; j < factors.length; ++j)
                line += " L" + factors[j] + "=" + Qt.lighter(colors[i], factors[j]) + " D" + factors[j] + "=" + Qt.darker(colors[i], factors[j])
            line += " a=" + Qt.alpha(colors[i], 0.3) + " t=" + Qt.tint(colors[i], "#40ff00ff") + " t2=" + Qt.tint(colors[i], "#c0102030")
            console.log(line)
        }
        for (var h = -1; h <= 1.0001; h += 0.125) {
            var row = "h" + h.toFixed(3) + ":"
            for (var s = 0; s <= 1.0001; s += 0.25)
                for (var l = 0; l <= 1.0001; l += 0.25)
                    row += " " + Qt.hsla(h, s, l, 1) + "/" + Qt.hsva(h, s, l, 0.5)
            console.log(row)
        }
        var row2 = "rgba:"
        for (var k = 0; k <= 20; ++k) row2 += " " + Qt.rgba(k / 20, 1 - k / 20, (k * 7 % 20) / 20, k / 20)
        console.log(row2)
        Qt.quit()
    }
}
