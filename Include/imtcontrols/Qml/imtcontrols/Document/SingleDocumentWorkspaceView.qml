import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtqml 1.0

Item {
    id: documentManager;

    anchors.fill: parent;

    property var startPageObj;

    property bool documentLoading: false;

    Component.onCompleted: {
        Events.subscribeEvent("DocumentUpdating", documentManager.documentUpdating);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("DocumentUpdating", documentManager.documentUpdating);
    }

    onStartPageObjChanged: {
        console.log("onStartPageObjChanged", documentManager.startPageObj["Source"]);
        documentLoader.source = documentManager.startPageObj["Source"];

        if (documentLoader.item.commandsId !== undefined){
            documentLoader.item.commandsId = documentManager.startPageObj["CommandsId"];
        }

        if(documentLoader.item.documentManager !== undefined){
            documentLoader.item.documentManager = documentManager;
        }
    }

    onDocumentLoadingChanged: {
        loading.visible = documentManager.documentLoading;
    }

    function documentUpdating(updatingFlag){
        console.log("documentUpdating", updatingFlag);

        if (!updatingFlag){
            documentManager.documentLoading = false;
        }
    }

    function documentSaved(parameters){
        let documentId = parameters["Id"];
        let documentName = parameters["Name"];

        if(documentLoader.item){
            documentLoader.item.updateGui();
        }

        // Get last document in stack view and setting data
        let item = stackView.peek();
        item.itemId = documentId;
        item.itemName = documentName;

        item.isDirty = false;
        item.saved();

        if (item.closingFlag){
            closeDocument(item.itemId);
        }
    }

    TreeItemModel {
        id: documentsData;
    }

    Loader {
        id: documentLoader;

        anchors.fill: parent;

        visible: stackView.countPage == 0;

        onLoaded: {
            console.log("onLoaded", documentLoader.source);
            if(documentLoader.item){
                if(documentLoader.item.documentManager !== undefined){
                    documentLoader.item.documentManager = documentManager;
                }
            }
        }
    }

    function addDocument(documentObj, params, isRequested){
        if (isRequested === undefined){
            isRequested = false;
        }

        let itemId = documentObj["Id"];
        documentManager.openDocument(itemId, documentObj, isRequested);
    }

    function openDocument(itemId, document, isRequested){
        documentManager.documentLoading = true;
        console.log("openDocument" , JSON.stringify(document));
        let keys = Object.keys(document);

        document["documentsData"] = documentsData;
        document["documentManager"] = documentManager;

        for (let key of keys){
            documentsData.SetData(key, document[key]);
        }

        let documentId = document["Id"];
        let commandId = document["CommandsId"];

        if (isRequested){
        }

        documentController.getData(documentId, document, commandId);

        stackView.push(document);
    }

    function saveDocument(documentId){
        console.log("saveDocument" ,documentId);

        let item = stackView.peek();

        item.updateModel();
        let documentData = item.documentModel;
        if (documentId === ""){
            documentController.setData(documentId, documentData);
        }
        else{
            documentController.updateData(documentId, documentData);
        }
    }

    function closeDocument(documentId){
        let document = stackView.peek();

        if (document && document.isDirty){
            modalDialogManager.openDialog(saveDialog, {"message": qsTr("Save all changes ?")});
        }
        else{
            stackView.pop();
        }
    }

    function setDocumentTitle(){
    }

    StackView {
        id: stackView;

        anchors.fill: parent;
    }

    Loading {
        id: loading;

        anchors.fill: parent;

        visible: false;
    }

    GqlDocumentDataController {
        id: documentController;

        onDocumentModelChanged: {
            if (documentController.documentModel != null){
                let item = stackView.peek();
                item.documentModel = documentController.documentModel;
            }
        }

        onDocumentAdded: {
            documentManager.documentSaved({"Id":documentId, "Name":documentName});
        }

        onDocumentUpdated: {
            documentManager.documentSaved({"Id":documentId, "Name":documentName});
        }

        onSavingError: {
            modalDialogManager.openDialog(savingErrorDialog, {"message": message});
        }
    }

    Component {
        id: savingErrorDialog;

        ErrorDialog {
            onFinished: {
            }
        }
    }

    Component {
        id: saveDialog;
        MessageDialog {
            Component.onCompleted: {
                console.log("saveDialog onCompleted");
                buttons.addButton({"Id":"Cancel", "Name":"Cancel", "Enabled": true});
            }

            onFinished: {
                console.log("saveDialog onFinished", buttonId);
                let document = stackView.peek();
                if (buttonId == "Yes"){
                    document.closingFlag = true;

                    documentManager.saveDocument(document.itemId);
                }
                else if (buttonId == "No"){
                    document.isDirty = false;
                    documentManager.closeDocument(document.itemId);
                }
            }
        }
    }
}
