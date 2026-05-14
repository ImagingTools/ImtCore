import QtQuick

MidListViewAlias {
    id: root

    Component.onCompleted: {
        root.headerText = "root"
        root.footerText = "leaf"
        console.log("combo21", root.baseCount, root.headerText, root.footerText, root.summary())
        Qt.quit()
    }
}