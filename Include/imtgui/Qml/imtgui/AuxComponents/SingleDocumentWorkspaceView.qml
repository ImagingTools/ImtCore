import QtQuick 2.12
import Acf 1.0

Item {
    id: documentManager;

    anchors.fill: parent;

    property var startPageObj;

    onStartPageObjChanged: {
        console.log("onStartPageObjChanged");
        documentLoader.source = documentManager.startPageObj["Source"];

        if (documentLoader.item){
            documentLoader.item.commandsId = documentManager.startPageObj["CommandsId"];
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
        }
    }

    function addDocument(document){
        let keys = Object.keys(document);

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
