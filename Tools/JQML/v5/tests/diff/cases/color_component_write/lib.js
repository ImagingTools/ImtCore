.pragma library
// a color changed in a JavaScript library
function faded(color, alpha) {
    var c = Qt.darker(color, 1)
    c.a = alpha
    return c
}
