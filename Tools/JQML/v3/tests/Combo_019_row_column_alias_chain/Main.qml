import QtQuick

MidRowColumn {
    id: root

    Component.onCompleted: {
        root.headerText = "root"
        root.detailText = "leaf"
        console.log("combo19", root.baseCount, root.headerText, root.detailText, root.summary())
        Qt.quit()
    }
}