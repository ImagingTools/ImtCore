import QtQuick

MidTimerState {
    id: root

    onBaseReady: {
        console.log("combo18-ready")
        console.log("combo18", root.baseTicks, root.midTicks, summary())
        Qt.quit()
    }

    Component.onCompleted: {
        root.baseTicks = 3
        root.startBase()
    }
}