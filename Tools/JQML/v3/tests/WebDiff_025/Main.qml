import QtQuick

Item {
    Loader {
        id: l
        sourceComponent: Rectangle {
            width: 40 + 25
            height: 20 + (25 % 7)
        }
    }

    Component.onCompleted: {
        console.log("wd025", "loader-size", l.width, l.height)
        Qt.quit()
    }
}
