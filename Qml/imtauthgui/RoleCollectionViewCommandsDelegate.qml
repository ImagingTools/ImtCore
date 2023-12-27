import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    removeDialogTitle: qsTr("Deleting a role");
    removeMessage: qsTr("Delete the selected role ?");

    autoUpdateAfterChanges: true;

    function updateCommands(){
        console.log("RoleColelction updateCommands");
    }

    function onEdit(){
        let itemIds = container.collectionViewBase.getSelectedIds();
        let itemNames = container.collectionViewBase.getSelectedNames();

        if (itemIds.length > 0){
            let itemId = itemIds[0];
            let itemName = itemNames[0];

            container.collectionViewBase.selectItem(itemId, itemName);
        }
    }

    function onRemove(){
        modalDialogManager.openDialog(removeDialog, {});
    }

    Component {
        id: removeDialog;
        MessageDialog {
            message: container.removeMessage;
            title: container.removeDialogTitle;
            onFinished: {
                if (buttonId == "Yes"){
                    let itemIds = container.collectionViewBase.getSelectedIds();
                    let itemNames = container.collectionViewBase.getSelectedNames();

                    for (let i = 0; i < itemIds.length; i++){
                        let itemId = itemIds[i];
                        let itemName = itemNames[i];

                        container.removeGqlModel.updateModel(itemId);
                    }
                }

                container.tableData.forceActiveFocus();
            }
        }
    }
}
