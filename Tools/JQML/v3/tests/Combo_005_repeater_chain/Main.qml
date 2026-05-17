import QtQuick

MidRepeater {
    id: root

    onAssembled: {
        console.log("combo5-main", combinedCount(), totalCount())
        Qt.quit()
    }
}
