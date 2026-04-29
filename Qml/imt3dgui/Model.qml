import QtQuick 2.15

Node3D {
    property string source: ""
    property var materials: undefined
    property bool castsShadows: true
    property bool receivesShadows: true
    property var geometry: undefined
    property var instancing: undefined
    property var instanceTable: undefined
    property var skeleton: undefined
    property var morphTargets: undefined
    property var skin: undefined
    property int pickable: 0
    property int depthBias: 0
}
