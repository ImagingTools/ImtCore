import QtQuick

Item {
    Loader {
        id: l
        sourceComponent: Rectangle {
            width: 40 + 85
            height: 20 + (85 % 7)
        }
    }

    Component.onCompleted: {
        console.log("wd085", "loader-size", l.width, l.height)
        Qt.quit()
    }
}
