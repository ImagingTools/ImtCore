import QtQuick

Item {
    Loader {
        id: l
        sourceComponent: Rectangle {
            width: 40 + 73
            height: 20 + (73 % 7)
        }
    }

    Component.onCompleted: {
        console.log("wd073", "loader-size", l.width, l.height)
        Qt.quit()
    }
}
