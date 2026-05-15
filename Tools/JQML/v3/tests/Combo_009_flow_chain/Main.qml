import QtQuick

MidFlow {
    id: root

    Component.onCompleted: {
        console.log("combo9-flow", details())
        Qt.quit()
    }
}
