import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    removeDialogTitle: qsTr("Deleting an account");
    removeMessage: qsTr("Delete the selected account ?");

//    function onSetDescription(){
//        let elements = container.tableData.elements;

//        let indexes = container.tableData.getSelectedIndexes();
//        if (indexes.length > 0){
//            let selectedDescription = "";
//            if (elements.ContainsKey("AccountDescription", indexes[0])){
//                selectedDescription = elements.GetData("AccountDescription", indexes[0]);
//            }

//            modalDialogManager.openDialog(container.setDescriptionDialogComp, {"message": qsTr("Please enter the description of the document:"), "inputValue": selectedDescription});
//        }
//    }
}

