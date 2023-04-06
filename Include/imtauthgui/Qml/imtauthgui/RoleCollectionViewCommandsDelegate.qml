import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    removeDialogTitle: qsTr("Deleting a role");
    removeMessage: qsTr("Delete the selected role ?");

    onCommandsProviderChanged: {
        commandsProvider.modelLoaded.connect(container.updateCommands);
    }

    Component.onDestruction: {
        commandsProvider.modelLoaded.disconnect(container.updateCommands);
    }

    function updateCommands(){
        console.log("RoleColelction updateCommands");

        let index = commandsProvider.commandsModel.InsertNewItem();

        commandsProvider.commandsModel.SetData("Id", "New", index);
        commandsProvider.commandsModel.SetData("IsEnabled", true, index);
        commandsProvider.commandsModel.SetData("Visible", false, index);
        commandsProvider.commandsModel.SetData("Status", "", index);
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
