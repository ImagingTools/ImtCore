import QtQuick 2.15

QtObject {
    property int maxAmount: 100
    property color color: "#ffffff"
    property real colorVariation: 0
    property bool unifiedColorVariation: false
    property real fadeInDuration: 0
    property real fadeOutDuration: 0
    property var delegate: undefined
    property real particleScale: 1
}
