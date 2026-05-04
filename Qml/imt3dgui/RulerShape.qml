import QtQuick 2.15

Node3D {
    id: rulerShape

    // Ruler endpoint positions in 3D space
    property vector3d startPoint: Qt.vector3d(0, 0, 0)
    property vector3d endPoint: Qt.vector3d(1, 0, 0)

    // Visual properties
    property color color: "#FF6600"                  // Orange (matches C++ s_color)
    property color selectionColor: "#0000FF"          // Blue (matches C++ s_selectionColor)
    property real lineWidth: 4.0
    property real pointSize: 12.0

    // Interaction state
    property bool isSelected: false
    property int selectedVertexIndex: -1              // -1=none, 0=start, 1=end

    // Reference shape for vertex snapping (analog to m_slaveShapePtr)
    property var slaveShape: undefined

    // Overlay label configuration
    property bool showLabels: true
    property bool showDistance: true

    // Computed distance between endpoints
    readonly property real distance: startPoint.minus(endPoint).length()
}
