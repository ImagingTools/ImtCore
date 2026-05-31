import QtQuick

Item {
    id: root

    property var source: null
    property int mirrored: -1
    property string last: ""
    property int dynCount: 0

    function dynTick(label) {
        dynCount += 1
        console.log("combo29-dyn", label, dynCount, mirrored)
    }

    Binding {
        target: root
        property: "mirrored"
        value: root.source ? root.source.value + 1 : -1
    }

    Connections {
        target: root.source
        function onTick(label) {
            root.last = label + ":" + root.mirrored
            console.log("combo29-conn", label, root.mirrored)
        }
    }
}
