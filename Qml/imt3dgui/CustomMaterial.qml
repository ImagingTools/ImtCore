import QtQuick 2.15

QtObject {
    readonly property int Opaque: 0
    readonly property int Transparent: 1

    property string vertexShader: ""
    property string fragmentShader: ""
    property int shadingMode: 0
    property int destinationBlend: 0
    property int sourceBlend: 0
    property bool alwaysDirty: false
    property var textureProperties: undefined
}
