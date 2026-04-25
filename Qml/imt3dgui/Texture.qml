/*
 * Texture - 2D image texture.
 *
 * Tiling mode constants (accessible via instance):
 *   ClampToEdge = 0, MirroredRepeat = 1, Repeat = 2
 */
import QtQuick 2.15

QtObject {
    readonly property int ClampToEdge: 0
    readonly property int MirroredRepeat: 1
    readonly property int Repeat: 2

    property string source: ""
    property int tilingModeHorizontal: 0 // ClampToEdge
    property int tilingModeVertical: 0   // ClampToEdge
}
