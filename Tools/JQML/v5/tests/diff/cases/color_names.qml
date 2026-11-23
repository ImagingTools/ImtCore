import QtQuick
// the SVG color names as QColor::fromString knows them (grey as gray, the SVG values of mediumpurple, palevioletred)
Item {
    property color a: "grey"
    property color b: "darkgrey"
    property color c: "dimgrey"
    property color d: "slategrey"
    property color e: "lightslategrey"
    property color f: "darkslategrey"
    property color g: "lightgrey"
    property color h: "mediumpurple"
    property color i: "palevioletred"
    Component.onCompleted: { console.log(a, b, c, d, e, f, g, h, i); Qt.quit() }
}
