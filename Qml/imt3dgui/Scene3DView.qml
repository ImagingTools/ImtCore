/**
 * \brief Real C++ implementation — backed by CScene3dItem.
 *
 * On the desktop this type is registered from C++ via
 * qmlRegisterType<CScene3dItem>("imt3dgui", 1, 0, "Scene3DView") and the
 * QML file here serves only as a fallback stub so that the JQML web
 * compiler can still parse the module.  At runtime on desktop the C++
 * type takes precedence; on the web the stub provides the property
 * interface but no real 3-D rendering.
 *
 * \sa CScene3dItem (Include/imt3dgui/CScene3dItem.h)
 */
import QtQuick 2.15

Item {
    property real cameraDistance: 3.0
    property real rotationX: 25.0
    property real rotationY: 45.0
    property color backgroundColor: "black"
}
