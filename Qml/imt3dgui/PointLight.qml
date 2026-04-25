// PointLight - omnidirectional point light with attenuation.
import QtQuick 2.15

Node3D {
    property color color: "#ffffff"
    property real brightness: 1.0
    property real constantFade: 1.0
    property real linearFade: 0.0
    property real quadraticFade: 1.0
    property bool castsShadow: false
}
