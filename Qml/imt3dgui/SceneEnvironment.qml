/*
 * SceneEnvironment - scene background, tone mapping and anti-aliasing.
 *
 * Enum-like constants are exposed as readonly properties. Reference them
 * through an instance id (e.g. `env.Color`) or use the integer values
 * directly (0 = Color, 1 = SkyBox, 2 = Transparent).
 */
import QtQuick 2.15

QtObject {
    // BackgroundMode
    readonly property int Color: 0
    readonly property int SkyBox: 1
    readonly property int Transparent: 2

    // AntialiasingMode
    readonly property int NoAA: 0
    readonly property int SSAA: 1
    readonly property int MSAA: 2

    // AntialiasingQuality
    readonly property int Medium: 0
    readonly property int High: 1
    readonly property int VeryHigh: 2

    // TonemapMode
    readonly property int NoTonemap: 0
    readonly property int TonemapModeLinear: 1
    readonly property int TonemapModeAces: 2
    readonly property int TonemapModeFilmic: 3

    property color clearColor: "#000000"
    property int backgroundMode: 0     // Color
    property var lightProbe
    property int tonemapMode: 1        // TonemapModeLinear
    property int antialiasingMode: 0   // NoAA
    property int antialiasingQuality: 0 // Medium
}
