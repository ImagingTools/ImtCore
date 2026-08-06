/**
 * \brief Web stub — QtQuick3D SpotLight replacement for JQML.
 *
 * Property-only stub; on the desktop the real QtQuick3D type is used.
 */
import QtQuick 2.15
import imt3dgui 1.0

Node3D {
    property color color: "#ffffff"
    property real brightness: 1
    property real coneAngle: 40
    property real innerConeAngle: 30
    property bool castsShadow: false
    property int shadowMapQuality: 1
    property real shadowBias: 0
    property real shadowMapFar: 5000
    property int bakeMode: 0
    property var scope: undefined
}
