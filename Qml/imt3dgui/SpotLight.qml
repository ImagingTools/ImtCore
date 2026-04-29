import QtQuick 2.15

Node3D {
    property color color: "#ffffff"
    property real brightness: 1
    property real coneAngle: 40
    property real innerConeAngle: 30
    property bool castsShadow: false
    property int shadowMapQuality: 1
    property real shadowBias: 0
    property real shadowMapFar: 5000
    property int bakeMode: 0
    property var scope: undefined
}
