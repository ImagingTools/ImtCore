import QtQuick

Item {
    Loader {
        id: l
        sourceComponent: ListView {
            id: v
            width: 90
            height: 40
            model: 4
            delegate: Rectangle { width: 15; height: 10 }
        }
    }

    Component.onCompleted: {
        console.log("wd058", "loader-listview-number", l.width, l.height)
        Qt.quit()
    }
}
