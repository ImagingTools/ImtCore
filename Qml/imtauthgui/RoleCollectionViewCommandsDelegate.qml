import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0

DocumentCollectionViewDelegate {
    id: container;

    removeDialogTitle: qsTr("Deleting a role");
    removeMessage: qsTr("Delete the selected role ?");

    function updateCommands(){
        console.log("RoleColelction updateCommands");
    }

    function onEdit(){
        let elementsModel = container.collectionView.table.elements;
        if (!elementsModel){
            return;
        }

        let indexes = container.collectionView.table.getSelectedIndexes();
        if (indexes.length > 0){
            let index = indexes[0];

            let itemId = elementsModel.GetData("Roles").GetData("Id", index);
            container.openDocumentEditor(itemId, documentTypeId, viewTypeId);
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
                if (buttonId == Enums.yes){
                    let itemIds = container.collectionViewBase.getSelectedIds();
                    let itemNames = container.collectionViewBase.getSelectedNames();

                    for (let i = 0; i < itemIds.length; i++){
                        let itemId = itemIds[i];

                        container.removeGqlModel.updateModel(itemId);
                    }
                }

                container.tableData.forceActiveFocus();
            }
        }
    }
}
