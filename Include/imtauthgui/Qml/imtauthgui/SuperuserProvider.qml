import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0

QtObject {
    id: root;

    signal result(bool exists);

    signal modelStateChanged(string state);

    property GqlDocumentDataController documentController: GqlDocumentDataController {
        onDocumentModelChanged: {
            console.log("onDocumentModelChanged", documentModel.toJSON());
            root.result(true);
        }

        onError: {
            console.log("onError", message);
            root.result(false);
        }

        onGetModelStateChanged: {
            root.modelStateChanged(this.getModelState);
        }
    }

    function superuserExists(){
        console.log("superuserExists");
        documentController.getData("su", {}, "User")
    }
}


