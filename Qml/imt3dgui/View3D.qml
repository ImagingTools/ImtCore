/**
 * \brief Web stub — QtQuick3D View3D replacement for the JQML compiler.
 *
 * This file is a pure-QML property stub that mirrors the public API of
 * QtQuick3D's View3D.  It carries no rendering logic of its own.
 *
 * - **Desktop:** not used; the real QtQuick3D View3D is loaded instead.
 * - **Web (JQML + three.js):** the JQML compiler translates this into a
 *   three.js WebGLRenderer/scene/camera setup.
 *
 * For a web-side software fallback see Render3D.qml which uses a QML
 * Canvas to draw a simplified 3-D scene without three.js.
 *
 * \sa Render3D, Scene3DView
 */
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
