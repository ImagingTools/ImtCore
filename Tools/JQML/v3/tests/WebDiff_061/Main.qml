import QtQuick

Item {
    Loader {
        id: l
        sourceComponent: Rectangle {
            width: 40 + 61
            height: 20 + (61 % 7)
        }
    }

    Component.onCompleted: {
        console.log("wd061", "loader-size", l.width, l.height)
        Qt.quit()
    }
}
