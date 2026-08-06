/**
 * \brief Web stub — QtQuick3D ParticleSystem3D replacement for JQML.
 *
 * Property-only stub; on the desktop the real QtQuick3D type is used.
 */
import QtQuick 2.15
import imt3dgui 1.0

Node3D {
    property bool running: true
    property bool paused: false
    property bool logging: false
    property int startTime: 0
    property real time: 0
    property bool useRandomSeed: true
    property int seed: 0
    default property list<QtObject> particles
}
