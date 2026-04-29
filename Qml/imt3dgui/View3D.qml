import QtQuick 2.15

Item {
    property var camera: undefined
    property var environment: undefined
    property var importScene: undefined
    property real renderMode: 0
    property bool ready3D: false
    property color clearColor: "#00000000"
    signal ready()
    signal sceneGraphChanged()
    default property list<QtObject> sceneNodes
}
