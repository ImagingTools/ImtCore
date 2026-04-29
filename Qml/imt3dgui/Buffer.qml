import QtQuick 2.15

QtObject {
    readonly property int Unknown: 0
    readonly property int UByte: 1
    readonly property int UByte4: 2
    readonly property int Short2: 3
    readonly property int Float16: 4
    readonly property int Float32: 5

    property string name: ""
    property int format: 0
    property int sizeMultiplier: 1
    property bool hasSceneLifetime: true
}
