import QtQuick

MidColumn {
    id: root

    Component.onCompleted: {
        root.headerText = "root"
        root.footerText = "leaf"
        console.log("combo13", root.details(), root.headerText, root.footerText, root.baseSpacing)
        Qt.quit()
    }
}