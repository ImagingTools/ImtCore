/**
 * \brief Web stub — QtQuick3D PrincipledMaterial replacement for JQML.
 *
 * Property-only stub; on the desktop the real QtQuick3D type is used.
 */
import QtQuick 2.15

QtObject {
    readonly property int Default: 0
    readonly property int Mask: 1
    readonly property int Blend: 2
    readonly property int Opaque: 3

    property color baseColor: "#ffffffff"
    property real metalness: 0
    property real roughness: 1
    property real opacity: 1
    property color emissiveColor: "#000000ff"
    property var baseColorMap: undefined
    property var metalnessMap: undefined
    property var roughnessMap: undefined
    property var normalMap: undefined
    property var emissiveMap: undefined
    property var occlusionMap: undefined
    property var heightMap: undefined
    property int alphaMode: 0
    property real alphaCutoff: 0.5
    property real normalStrength: 1
    property real specularAmount: 1
    property color specularTint: "#ffffffff"
    property real clearcoatAmount: 0
    property real clearcoatRoughnessAmount: 0
    property real transmissionFactor: 0
    property real pointSize: 1
    property real lineWidth: 1
}
