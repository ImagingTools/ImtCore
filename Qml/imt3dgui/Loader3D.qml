import QtQuick 2.15

Node3D {
    property var sourceComponent: undefined
    property string source: ""
    property bool asynchronous: true
    property bool active: true
    property var item: undefined

    readonly property int Null: 0
    readonly property int Ready: 1
    readonly property int Loading: 2
    readonly property int Error: 3

    property int status: 0
}
