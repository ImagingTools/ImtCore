import QtQuick

Item {
    Component.onCompleted: {
        console.log("file %1 slot %2".arg("ok").arg(2))
        Qt.quit()
    }
}
