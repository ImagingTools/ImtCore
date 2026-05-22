import QtQuick

MidModelRepeater {
    id: root

    Component.onCompleted: {
        console.log("combo16", summary(), root.modelCount(), root.extraCount)
        Qt.quit()
    }
}