import QtQuick

MidNode {
    id: root

    property int dynSum: 0
    property string trace: ""

    function dynHandler(step) {
        dynSum += step
        console.log("combo28-dyn", step, dynSum, mirrored)
    }

    signal done()

    Connections {
        target: root
        function onMidNotified(message) {
            root.trace = root.trace + "|" + message
            console.log("combo28-conn", message, root.mirrored)
        }
    }

    onDone: Qt.quit()

    Component.onCompleted: {
        bump.connect(dynHandler)

        doBump(1)
        doBump(2)

        bump.disconnect(dynHandler)

        doBump(3)

        console.log("combo28-final", baseValue, mirrored, dynSum, trace)
        done()
    }
}
