import QtQuick

MidTextEdit {
    id: root

    Component.onCompleted: {
        console.log("combo10-text", mergedText())
        Qt.quit()
    }
}
