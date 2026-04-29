import QtQuick 2.15

QtObject {
    readonly property int Position: 0
    readonly property int Normal: 1
    readonly property int TexCoord0: 2
    readonly property int TexCoord1: 3
    readonly property int Tangent: 4
    readonly property int Binormal: 5
    readonly property int JointIndex: 6
    readonly property int JointWeight: 7
    readonly property int Color: 8

    readonly property int UnsignedInt16: 0
    readonly property int UnsignedInt32: 1

    readonly property int Float32: 0

    property string name: ""
    property int primitiveType: 0
    property var vertexData: undefined
    property var indexData: undefined
    property int stride: 0
    property var attributes: undefined
}
