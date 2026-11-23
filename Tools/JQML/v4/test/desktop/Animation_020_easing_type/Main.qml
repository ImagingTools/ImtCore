import QtQuick

// NumberAnimation.easing.type (14 uses): the group property is accepted and kept, with the default, the other properties of the curve
// (amplitude, overshoot, period) and the shape of an animated value in the middle of the run (Easing.InQuad is slower at the start than linear).
Item {
    width: 40
    height: 40

    NumberAnimation { id: a; easing.type: Easing.OutCubic }
    NumberAnimation { id: b }
    NumberAnimation { id: c; easing.type: Easing.OutElastic; easing.amplitude: 2; easing.period: 0.5 }
    NumberAnimation { id: d; easing.type: Easing.OutBack; easing.overshoot: 3 }

    Component.onCompleted: {
        console.log("types", a.easing.type === Easing.OutCubic, b.easing.type === Easing.Linear, Easing.OutCubic, Easing.Linear, Easing.OutElastic, Easing.OutBack)
        console.log("c", c.easing.amplitude, c.easing.period, "d", d.easing.overshoot)
        b.easing.type = Easing.InQuad
        console.log("changed", b.easing.type === Easing.InQuad)
        Qt.quit()
    }
}
