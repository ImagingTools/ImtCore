import QtQuick 2.15

QtObject {
    readonly property int ClampToEdge: 0
    readonly property int MirroredRepeat: 1
    readonly property int Repeat: 2

    property string source: ""
    property int tilingModeHorizontal: 0
    property int tilingModeVertical: 0
}
