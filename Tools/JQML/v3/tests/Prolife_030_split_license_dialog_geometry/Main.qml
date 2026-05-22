import QtQuick 2.0

// Prolife_030: SplitLicenseDialog geometry
// fixed width=800, height=500
// contentComp: width=dialog.width, height=dialog.height-100
// default properties: spinBoxValue=1, createNewMode=true, licenseId=""
//                     maxAvailableCount=0, selectedAccountId="", selectedTargetLicenseId=""

Item {
    id: root
    width: 1200
    height: 800

    Item {
        id: dialog

        property int spinBoxValue: 1
        property bool createNewMode: true
        property string licenseId: ""
        property int maxAvailableCount: 0
        property string selectedAccountId: ""
        property string selectedTargetLicenseId: ""

        width: 800
        height: 500

        Item {
            id: contentComp
            width: dialog.width
            height: dialog.height - 100
        }
    }

    Component.onCompleted: {
        console.log("prolife30",
            dialog.width, dialog.height,
            contentComp.width, contentComp.height,
            dialog.spinBoxValue,
            dialog.createNewMode,
            dialog.licenseId === "" ? "empty" : dialog.licenseId,
            dialog.maxAvailableCount,
            dialog.selectedAccountId === "" ? "empty" : dialog.selectedAccountId,
            dialog.selectedTargetLicenseId === "" ? "empty" : dialog.selectedTargetLicenseId
        )
        Qt.quit()
    }
}
