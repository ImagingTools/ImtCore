import QtQuick

MidLoaderText {
    id: root

    onReady: {
        console.log("combo15", summary(), loaderState(), root.suffix)
        Qt.quit()
    }
}