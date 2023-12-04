import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Dialog {
    id: documentHistoryDialog;

    width: 500;

    property string documentTypeId: "";
    property string documentId: "";

    Component.onCompleted: {
        buttons.addButton({"Id": "Ok", "Name": qsTr("OK"), "Enabled": true});
    }

    function updateModel(){
        documentHistoryDialog.bodyItem.updateModel(documentTypeId, documentId);
    }

    contentComp: Component {
        id: productPairEditor;

        DocumentHistoryView {
            height: 500;
        }
    }
}

