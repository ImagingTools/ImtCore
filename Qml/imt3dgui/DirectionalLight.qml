import QtQuick 2.15

Node3D {
    readonly property int ShadowMapQualityLow: 0
    readonly property int ShadowMapQualityMedium: 1
    readonly property int ShadowMapQualityHigh: 2
    readonly property int ShadowMapQualityVeryHigh: 3

    property color color: "#ffffff"
    property real brightness: 1
    property bool castsShadow: false
    property int shadowMapQuality: 1
    property real shadowBias: 0
    property real shadowMapFar: 5000
    property int bakeMode: 0
    property var scope: undefined
    property color shadowColor: "#000000"
    property real shadowFactor: 5
}
