import QtQuick 2.0

// Prolife_027: LicenseTreeDialog geometry
// width = parent.width * 0.95  (fallback 1200 if parent not available)
// height = parent.height * 0.95 (fallback 800)
// contentComp: width=dialog.width, height=dialog.height-100
// default properties: treeData=null, currentLicenseId=""

Item {
    id: root
    width: 1400
    height: 900

    Item {
        id: dialog

        property int treeDataCount: 0
        property string currentLicenseId: ""

        width: root.width * 0.95
        height: root.height * 0.95

        Item {
            id: contentComp
            width: dialog.width
            height: dialog.height - 100
        }
    }

    Component.onCompleted: {
        console.log("prolife27",
            dialog.width, dialog.height,
            contentComp.width, contentComp.height,
            dialog.treeDataCount,
            dialog.currentLicenseId.length
        )
        Qt.quit()
    }
}
