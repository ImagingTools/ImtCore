import QtQuick 2.0

// Prolife_029: RevokeLicenseDialog geometry
// fixed width=900, height=600
// contentComp: width=dialog.width, height=dialog.height-100
// default properties: revokeCount=1, selectedAvailableCount=0, selectedBoundCount=0
//                     parentLicenseId="", selectedChildLicenseId=""

Item {
    id: root
    width: 1200
    height: 800

    Item {
        id: dialog

        property int revokeCount: 1
        property int selectedAvailableCount: 0
        property int selectedBoundCount: 0
        property string parentLicenseId: ""
        property string selectedChildLicenseId: ""

        width: 900
        height: 600

        Item {
            id: contentComp
            width: dialog.width
            height: dialog.height - 100
        }
    }

    Component.onCompleted: {
        console.log("prolife29",
            dialog.width, dialog.height,
            contentComp.width, contentComp.height,
            dialog.revokeCount,
            dialog.selectedAvailableCount,
            dialog.selectedBoundCount,
            dialog.parentLicenseId === "" ? "empty" : dialog.parentLicenseId,
            dialog.selectedChildLicenseId === "" ? "empty" : dialog.selectedChildLicenseId
        )
        Qt.quit()
    }
}
