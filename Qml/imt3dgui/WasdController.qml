import QtQuick 2.15

Item {
    property var controlledObject: undefined
    property real speed: 1
    property real shiftSpeed: 3
    property real forwardSpeed: 1
    property real backSpeed: 1
    property real rightSpeed: 1
    property real leftSpeed: 1
    property real upSpeed: 1
    property real downSpeed: 1
    property real xSpeed: 0.1
    property real ySpeed: 0.1
    property bool mouseEnabled: true
    property bool keysEnabled: true
    property bool inputsNeedProcessing: false
    property bool acceptedButtons: true
}
