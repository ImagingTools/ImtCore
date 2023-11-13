import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    removeDialogTitle: qsTr("Deleting an user");
    removeMessage: qsTr("Delete the selected user ?");

    autoUpdateAfterChanges: true;

    function onEdit(){
        console.log("UserCollectionView onEdit");

        let itemIds = container.tableData.getSelectedIds();
        let itemNames = container.tableData.getSelectedNames();

        console.log("itemIds", itemIds);

        if (itemIds.length > 0){
            let itemId = itemIds[0];
            let itemName = itemNames[0];

            container.collectionViewBase.selectItem(itemId, itemName);
        }
    }
}
