import QtQuick

Item {
    id: root
    signal ping(int value)

    onPing: {
        console.log("ping", value)
        Qt.quit()
    }

    Component.onCompleted: root.ping(7)
}
