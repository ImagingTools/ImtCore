import QtQuick

Item {
    Loader {
        id: l
        sourceComponent: Rectangle {
            width: 40 + 97
            height: 20 + (97 % 7)
        }
    }

    Component.onCompleted: {
        console.log("wd097", "loader-size", l.width, l.height)
        Qt.quit()
    }
}
