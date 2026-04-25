/*
 * Model - mesh-rendering 3D node.
 *
 * Supports the same primitive tokens as the JQML web runtime: "#Cube",
 * "#Sphere", "#Cylinder", "#Cone", "#Plane", "#Rectangle".
 * URLs ending in .glb/.gltf are treated as glTF assets.
 */
import QtQuick 2.15

Node3D {
    property string source: ""
    property list<QtObject> materials
    property bool castsShadows: true
    property bool receivesShadows: true
    property var geometry
}
