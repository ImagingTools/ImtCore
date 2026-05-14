import QtQuick

Item {
    Loader {
        id: l
        sourceComponent: Rectangle {
            width: 40 + 37
            height: 20 + (37 % 7)
        }
    }

    Component.onCompleted: {
        console.log("wd037", "loader-size", l.width, l.height)
        Qt.quit()
    }
}
