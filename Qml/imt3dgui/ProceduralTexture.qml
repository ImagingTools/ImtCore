import QtQuick 2.15

QtObject {
    property int width: 256
    property int height: 256

    readonly property int RGBA8: 0
    readonly property int RGBA16F: 1
    readonly property int RGBA32F: 2

    property int format: 0
    property int tilingModeHorizontal: 0
    property int tilingModeVertical: 0

    signal textureDataUpdated()
}
