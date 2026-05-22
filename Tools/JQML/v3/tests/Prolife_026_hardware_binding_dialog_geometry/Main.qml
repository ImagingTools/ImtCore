import QtQuick 2.0

// Prolife_026: HardwareProductBindingDialog geometry
// dialogDefaultWidth = 1000
// height = activeView.height - 200
// width = (rootWidth < dialogDefaultWidth) ? rootWidth : dialogDefaultWidth
// contentComp: width=dialog.width, height=dialog.height-100
// default properties: softwareIds=[], hardwareId="", bindingModel=null

Item {
    id: root
    width: 1280
    height: 900

    // Simulate ModalDialogManager.activeView
    Item {
        id: activeView
        width: root.width
        height: root.height

        // Dialog geometry
        Item {
            id: dialog

            property int dialogDefaultWidth: 1000
            property var softwareIds: []
            property string hardwareId: ""
            property int revokeCount: 0

            width: (root.width < dialogDefaultWidth) ? root.width : dialogDefaultWidth
            height: activeView.height - 200

            // contentComp
            Item {
                id: contentComp
                width: dialog.width
                height: dialog.height - 100
            }
        }
    }

    Component.onCompleted: {
        console.log("prolife26",
            dialog.dialogDefaultWidth,
            dialog.width, dialog.height,
            contentComp.width, contentComp.height,
            dialog.softwareIds.length,
            dialog.hardwareId,
            dialog.revokeCount
        )
        Qt.quit()
    }
}
