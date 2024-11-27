import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Dialog {
    id: documentHistoryDialog;

    property string documentTypeId: "";
    property string documentId: "";

    function updateModel(){
        documentHistoryDialog.bodyItem.updateModel(documentTypeId, documentId);
    }

    Component.onCompleted: {
        buttonsModel.append({Id: Enums.ok, Name:qsTr("OK"), Enabled: true})
    }

    contentComp: Component {
        id: productPairEditor;

        DocumentHistoryView {
            width: 500;
            height: 500;
        }
    }
}

