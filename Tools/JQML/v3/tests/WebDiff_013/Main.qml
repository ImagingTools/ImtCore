import QtQuick

Item {
    Loader {
        id: l
        sourceComponent: Rectangle {
            width: 40 + 13
            height: 20 + (13 % 7)
        }
    }

    Component.onCompleted: {
        console.log("wd013", "loader-size", l.width, l.height)
        Qt.quit()
    }
}
