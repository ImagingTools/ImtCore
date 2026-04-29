import QtQuick 2.15

QtObject {
    readonly property int ClampToEdge: 0
    readonly property int MirroredRepeat: 1
    readonly property int Repeat: 2

    readonly property int UV0: 0
    readonly property int UV1: 1
    readonly property int Environment: 2
    readonly property int LightProbe: 3

    readonly property int None: 0
    readonly property int Linear: 1
    readonly property int Nearest: 2

    property string source: ""
    property int tilingModeHorizontal: 0
    property int tilingModeVertical: 0
    property bool flipU: false
    property bool flipV: false
    property real scaleU: 1
    property real scaleV: 1
    property real pivotU: 0
    property real pivotV: 0
    property real rotationUV: 0
    property int mappingMode: 0
    property int minFilter: 0
    property int magFilter: 0
    property int mipFilter: 0
    property bool generateMipmaps: false
}
