import QtQuick 2.12
import Acf 1.0

Item {
    id: documentManager;

    anchors.fill: parent;

    property var startPageObj;

    Component.onCompleted: {
        Events.subscribeEvent("DocumentSaved", documentManager.documentSaved);
        Events.subscribeEvent("CloseDocument", documentManager.closeDocument);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("DocumentSaved", documentManager.documentSaved);
        Events.unSubscribeEvent("CloseDocument", documentManager.closeDocument);
    }

    onStartPageObjChanged: {
        console.log("onStartPageObjChanged", documentManager.startPageObj["Source"]);
        documentLoader.source = documentManager.startPageObj["Source"];

//        if (documentLoader.item){
//            documentLoader.item.commandsId = documentManager.startPageObj["CommandsId"];
//        }

//        console.log("documentLoader.item", documentLoader.item);
        if(documentLoader.item.documentManager !==undefined){
            documentLoader.item.documentManager = documentManager;
            console.log("documentLoader.item.documentManager", documentLoader.item.documentManager);
        }
    }

    function documentSaved(parameters){
        if(documentLoader.item){
            documentLoader.item.updateGui();
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

    function addDocument(document){
        let keys = Object.keys(document);

        document["documentsData"] = documentsData;
        document["documentManager"] = documentManager;

        for (let key of keys){
            documentsData.SetData(key, document[key]);
        }

        documentsData.SetData("Title", document["Name"]);

        stackView.push(document);
    }

    function closeDocument(){
        stackView.pop();
    }

    function setDocumentTitle(){
    }

    StackView {
        id: stackView;

        anchors.fill: parent;
    }
}
