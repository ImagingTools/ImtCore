import QtQuick

MidLoaderTimer {
    id: root

    signal mainReady()

    onBaseReady: {
        console.log("combo20-base")
    }

    onMidReady: {
        console.log("combo20-mid")
        mainReady()
    }

    onMainReady: {
        console.log("combo20-main")
        console.log("combo20", root.baseState, root.midState, summary())
        Qt.quit()
    }

    Component.onCompleted: {
        root.baseState = 4
        root.startAll()
    }
}