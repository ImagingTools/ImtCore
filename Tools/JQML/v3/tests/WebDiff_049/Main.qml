import QtQuick

Item {
    Loader {
        id: l
        sourceComponent: Rectangle {
            width: 40 + 49
            height: 20 + (49 % 7)
        }
    }

    Component.onCompleted: {
        console.log("wd049", "loader-size", l.width, l.height)
        Qt.quit()
    }
}
