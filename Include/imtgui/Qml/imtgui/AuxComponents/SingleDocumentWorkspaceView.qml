import QtQuick 2.12
import Acf 1.0

Item {
    id: documentManager;

    anchors.fill: parent;

    property var startPageObj;

    onStartPageObjChanged: {
        console.log("onStartPageObjChanged");
        documentLoader.source = startPageObj["Source"];

        if (documentLoader.item){
            documentLoader.item.commandsId = startPageObj["CommandsId"];
        }
    }

    TreeItemModel {
        id: documentsData;
    }

    Loader {
        id: documentLoader;

        anchors.fill: parent;

        onLoaded: {
            console.log("onLoaded", source);
        }
    }

    function addDocument(document){

        documentsData.SetData("ItemId", document["Id"]);
        documentsData.SetData("Title", document["Name"]);
        documentsData.SetData("Source", document["Source"]);
        documentsData.SetData("CommandsId", document["CommandsId"]);

        stackView.push(document);
    }

    StackView {
        id: stackView;

        anchors.fill: parent;
    }
}
