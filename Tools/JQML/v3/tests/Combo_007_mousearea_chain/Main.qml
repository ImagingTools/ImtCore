import QtQuick

MidMouseArea {
    id: root

    Component.onCompleted: {
        console.log("combo7-main", inspect(), hitCount)
        Qt.quit()
    }
}
