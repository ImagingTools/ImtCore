import QtQuick
import "lib.js" as Lib

// the components of a color written: QQuickColorValueType setters (Style.alphaColor of the Tesla dashboard)
// (v5 keeps 8 bits a channel: the components are printed with 2 decimals)
Item {
    readonly property color black: "#000000"
    property color target: "red"
    property var plain: ({ a: 1, r: 2 })
    function alphaColor(color, alpha) {
        let actualColor = Qt.darker(color, 1)
        actualColor.a = alpha
        return actualColor
    }
    function f2(x) { return Number(x).toFixed(2) }
    Rectangle { id: rect; color: alphaColor(black, 0.55) }
    Component.onCompleted: {
        let c = alphaColor(black, 0.55)
        console.log(c, f2(c.a), rect.color)
        // a property written back
        target.a = 0.5
        console.log(target, f2(target.a))
        rect.color.r = 1
        console.log(rect.color)
        const k = Qt.rgba(0.2, 0.4, 0.6, 1)
        k.g = 1
        console.log(k)
        let l = Qt.lighter("#336699", 1.0)
        l.r = 1
        console.log(l, f2(l.r), f2(l.g))
        let h = Qt.rgba(0.2, 0.4, 0.6, 1)
        h.hslLightness = 0.5
        let v = Qt.rgba(0.2, 0.4, 0.6, 1)
        v.hsvHue = 0.5
        let s = Qt.rgba(0.2, 0.4, 0.6, 1)
        s.hsvSaturation = 0.25
        console.log(h, v, s)
        // not a color: the member as JavaScript sets it
        plain.a = 5
        let o = { r: 1 }
        o.r = 7
        console.log(plain.a, plain.r, o.r, (c.b = 1) === 1)
        console.log(Lib.faded("#ff0000", 0.5))
        Qt.quit()
    }
}
