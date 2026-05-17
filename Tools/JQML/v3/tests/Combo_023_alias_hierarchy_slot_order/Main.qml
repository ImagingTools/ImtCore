import QtQuick

TopAliasNode {
    id: root

    onFinished: {
        console.log("combo23-final", topLabel)
        Qt.quit()
    }

    Component.onCompleted: {
        console.log("combo23-before", topLabel)
        topLabel = "alias-updated"
        console.log("combo23-after", topLabel)
        topStep()
    }
}