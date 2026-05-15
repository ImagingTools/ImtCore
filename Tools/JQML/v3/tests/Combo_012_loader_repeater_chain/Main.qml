import QtQuick

MidLoader {
    id: root

    onLoadedReady: {
        console.log("combo12", summary(), loadedSize(), root.bonusCount)
        Qt.quit()
    }
}