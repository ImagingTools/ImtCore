import QtQuick 2.15

Node3D {
    id: pointCloudShape

    // Geometry data source
    property var geometry: undefined                  // Geometry element with point data

    // Visual properties
    property color color: "#00CC33"                   // Green-cyan (matches C++ default 0.0, 0.8, 0.2)
    property color selectionColor: "#CCCC00"           // Yellow (matches C++ s_selectionColor)
    property real pointSize: 1.0

    // Selection state
    property var selectedIndices: []                   // Selected vertex indices
    property bool isSelected: false

    // Selection modes (analog to C++ selection methods)
    readonly property int selectionModePoint: 0
    readonly property int selectionModeBox: 1
    readonly property int selectionModeCircle: 2
    property int selectionMode: selectionModePoint

    // Info box overlay
    property bool showInfoBox: false
}
