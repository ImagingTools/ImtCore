import QtQuick

Item {
    id: root
    property bool usePrimary: true
    property int primary: 10
    property int secondary: 20

    Derived { id: measured; measured: root.usePrimary ? root.primary : root.secondary }
    Connections {
        target: measured
        function onMeasuredChanged(){ console.log("changed", measured.measured) }
    }
    Component.onCompleted: {
        console.log("initial", measured.size, measured.metric, measured.measured)
        root.secondary = 22
        root.usePrimary = false
        root.primary = 11
        root.secondary = 23
        measured.metric = 30
        root.secondary = 24
        console.log("detached", measured.measured)
        measured.measured = Qt.binding(function(){ return root.primary + root.secondary })
        root.primary = 12
        console.log("bound again", measured.measured)
        Qt.quit()
    }
}
