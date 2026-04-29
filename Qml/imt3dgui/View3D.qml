import QtQuick 2.15

Item {
    property var camera: undefined
    property var environment: undefined
    property var importScene: undefined
    property real renderMode: 0
    property bool ready3D: false
    property color clearColor: "#00000000"
    property int renderFormat: 0
    property int samples: 1
    property var renderStats: undefined
    signal ready()
    signal sceneGraphChanged()
    default property list<QtObject> sceneNodes

    function pick(x, y) { return undefined }
    function mapFrom3DScene(scenePos) { return Qt.point(0, 0) }
    function mapTo3DScene(viewPos) { return Qt.vector3d(0, 0, 0) }
}
