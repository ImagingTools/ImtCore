import QtQuick 2.15

Node3D {
    id: meshShape

    // Geometry data source
    property var geometry: undefined                  // Geometry element with triangle data
    property var materials: undefined                 // Material or list of materials

    // Color configuration
    property color color: "#00FFFF"                   // Cyan (matches C++ default)
    property color selectionColor: "#800080"           // Purple (matches C++ s_selectionColor)

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

    // Shadow configuration
    property bool castsShadows: true
    property bool receivesShadows: true
}
