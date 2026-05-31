import QtQuick

MidHost {
    id: root

    signal mainReady()

    onLoadedReady: {
        console.log("combo3-loaded", summary())
        mainReady()
    }

    onMainReady: {
        Qt.quit()
    }
}
