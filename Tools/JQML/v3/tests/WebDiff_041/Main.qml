import QtQuick

Item {
    property Component compA: Rectangle { width: 30 + 41; height: 15 }
    property Component compB: Rectangle { width: 10; height: 8 + (41 % 5) }

    Loader { id: l; sourceComponent: compA }

    Component.onCompleted: {
        l.sourceComponent = compB
        console.log("wd041", "loader-switch", l.width, l.height)
        Qt.quit()
    }
}
