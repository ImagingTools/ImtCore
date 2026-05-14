import QtQuick

BaseNode {
    id: root

    property int factor: 2
    property int mirrored: 0

    signal midNotified(string message)

    Binding {
        target: root
        property: "mirrored"
        value: root.baseValue * root.factor
    }

    Connections {
        target: root
        function onBump(step) {
            root.midNotified("mid:" + step + ":" + root.mirrored)
        }
    }
}
