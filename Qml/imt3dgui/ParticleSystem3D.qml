import QtQuick 2.15

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
