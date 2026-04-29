import QtQuick 2.15

QtObject {
    readonly property int Color: 0
    readonly property int SkyBox: 1
    readonly property int Transparent: 2

    readonly property int NoTonemap: 0
    readonly property int TonemapModeLinear: 1
    readonly property int TonemapModeAces: 2
    readonly property int TonemapModeFilmic: 3

    readonly property int NoAA: 0
    readonly property int SSAA: 1
    readonly property int MSAA: 2

    readonly property int Medium: 0
    readonly property int High: 1
    readonly property int VeryHigh: 2

    property color clearColor: "#000000ff"
    property int backgroundMode: 0
    property var lightProbe: undefined
    property int tonemapMode: 1
    property int antialiasingMode: 0
    property int antialiasingQuality: 0
}
