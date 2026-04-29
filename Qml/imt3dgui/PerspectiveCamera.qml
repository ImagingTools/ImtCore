import QtQuick 2.15

Node3D {
    property real fieldOfView: 60
    property real clipNear: 10
    property real clipFar: 10000
    property bool frustumCullingEnabled: false
    property var lookAtNode: undefined
}
