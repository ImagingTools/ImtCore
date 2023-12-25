import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolqml 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    removeDialogTitle: qsTr("Deleting an group of users");
    removeMessage: qsTr("Delete the selected group of users ?");

    autoUpdateAfterChanges: true;

    function onEdit(){
        let itemIds = container.tableData.getSelectedIds();
        let itemNames = container.tableData.getSelectedNames();

        if (itemIds.length > 0){
            let itemId = itemIds[0];
            let itemName = itemNames[0];

            container.collectionViewBase.selectItem(itemId, itemName);
        }
    }
}
