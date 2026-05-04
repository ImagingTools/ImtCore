import QtQuick 2.15

Node3D {
    id: axisShape

    // Axis type enumeration
    readonly property int axisX: 0
    readonly property int axisY: 1
    readonly property int axisZ: 2

    // Per-axis colors (matches C++ defaults: R/G/B)
    property color xAxisColor: "#FF0000"              // Red
    property color yAxisColor: "#00FF00"              // Green
    property color zAxisColor: "#0000FF"              // Blue

    // Per-axis lengths
    property real xAxisLength: 1.0
    property real yAxisLength: 1.0
    property real zAxisLength: 1.0

    // Per-axis line widths
    property real xAxisLineWidth: 2.0
    property real yAxisLineWidth: 2.0
    property real zAxisLineWidth: 2.0

    // Per-axis labels
    property string xAxisLabel: "X"
    property string yAxisLabel: "Y"
    property string zAxisLabel: "Z"

    // Per-axis value ranges
    property real xAxisRangeMin: 0.0
    property real xAxisRangeMax: 1.0
    property real yAxisRangeMin: 0.0
    property real yAxisRangeMax: 1.0
    property real zAxisRangeMin: 0.0
    property real zAxisRangeMax: 1.0

    // Label display
    property bool showLabels: true
}
