/**
 * \brief Web stub — QtQuick3D SpriteParticle3D replacement for JQML.
 *
 * Property-only stub; on the desktop the real QtQuick3D type is used.
 */
import QtQuick 2.15

QtObject {
    readonly property int Normal: 0
    readonly property int Screen: 1

    property int maxAmount: 100
    property color color: "#ffffff"
    property var colorTable: undefined
    property real colorVariation: 0
    property bool unifiedColorVariation: false
    property real fadeInDuration: 0
    property real fadeOutDuration: 0
    property int blendMode: 0
    property var sprite: undefined
    property int spriteSequenceFrameCount: 1
    property bool billboard: false
    property real particleScale: 1
}
