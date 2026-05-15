import QtQuick

Item {
    Loader {
        id: l
        sourceComponent: Rectangle {
            width: 40 + 1
            height: 20 + (1 % 7)
        }
    }

    Component.onCompleted: {
        console.log("wd001", "loader-size", l.width, l.height)
        Qt.quit()
    }
}
