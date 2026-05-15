import QtQuick

Item {
    Loader {
        id: l
        sourceComponent: Rectangle {
            width: 40 + 109
            height: 20 + (109 % 7)
        }
    }

    Component.onCompleted: {
        console.log("wd109", "loader-size", l.width, l.height)
        Qt.quit()
    }
}
