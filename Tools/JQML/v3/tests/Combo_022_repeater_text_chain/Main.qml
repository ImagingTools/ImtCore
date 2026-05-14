import QtQuick

MidRepeaterText {
    id: root

    Component.onCompleted: {
        console.log("combo22", root.baseCount, root.bonusCount, root.summary())
        Qt.quit()
    }
}