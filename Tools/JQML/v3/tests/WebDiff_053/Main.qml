import QtQuick

Item {
    property Component compA: Rectangle { width: 30 + 53; height: 15 }
    property Component compB: Rectangle { width: 10; height: 8 + (53 % 5) }

    Loader { id: l; sourceComponent: compA }

    Component.onCompleted: {
        l.sourceComponent = compB
        console.log("wd053", "loader-switch", l.width, l.height)
        Qt.quit()
    }
}
