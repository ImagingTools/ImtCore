import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Dialog {
    id: documentHistoryDialog;

    buttonsModel: ListModel{
        ListElement{Id: Enums.ButtonType.Ok; Name:qsTr("OK"); Enabled: true}
    }

//    width: 500;

    property string documentTypeId: "";
    property string documentId: "";

    function updateModel(){
        documentHistoryDialog.bodyItem.updateModel(documentTypeId, documentId);
    }

    contentComp: Component {
        id: productPairEditor;

        DocumentHistoryView {
            width: 500;
            height: 500;
        }
    }
}

