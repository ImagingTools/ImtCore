import QtQuick

BaseTextEdit {
    id: root

    property string suffix: "beta"

    function mergedText() {
        return currentText() + ":" + suffix
    }
}
