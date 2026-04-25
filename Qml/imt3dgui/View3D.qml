/*
 * View3D - 3D scene host inside a 2D Item rectangle.
 *
 * The imt3dgui module exposes 3D types under the same names that the JQML
 * web compiler / runtime understands (see Tools/JQML/v2/components/View3D.js).
 * Applications can therefore write `import imt3dgui 1.0` and use a single
 * QML source tree on both desktop and web (backed by three.js).
 *
 * On the desktop the actual 3D rendering is performed by the C++ backend
 * (CRhiWidget / COpenGLWidget).  This QML type is a pure declarative
 * scene-description container.
 */
import QtQuick 2.15

Item {
    property var camera
    property var environment
    property var importScene
    property int renderMode: 0
    property color clearColor: "transparent"
}
