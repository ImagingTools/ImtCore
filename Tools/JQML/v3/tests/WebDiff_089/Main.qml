import QtQuick

Item {
    property Component compA: Rectangle { width: 30 + 89; height: 15 }
    property Component compB: Rectangle { width: 10; height: 8 + (89 % 5) }

    Loader { id: l; sourceComponent: compA }

    Component.onCompleted: {
        l.sourceComponent = compB
        console.log("wd089", "loader-switch", l.width, l.height)
        Qt.quit()
    }
}
