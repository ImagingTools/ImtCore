import QtQuick 2.12
import Acf 1.0

QtObject {
    id: root;

    property var documentModel;

    property string documentId;
    property string documentName;
	property string typeId;

    property bool hasRemoteChanges: false;

    property Component documentModelComp;

    signal saved(string id, string name);
    signal error(string message, string type);
    signal modelChanged();

	Component.onDestruction: {
		if (documentModel){
			documentModel.destroy();
		}
	}

    onDocumentModelChanged: {
        setupDocumentInfo();

        hasRemoteChanges = false;
        modelChanged();
    }

    onSaved: {
        documentId = id;
        documentName = name;
    }

    function getDocumentId(){
        return documentId;
    }

    function getDocumentName(){
        return documentName;
    }

    function updateDocumentModel(){
        modelChanged();
    }

    function insertDocument(){
    }

    function saveDocument(){
    }

    // For setted documentId/documentName from documentModel
    function setupDocumentInfo(){}

    function createDocumentModel(){
        if (root.documentModelComp){
			documentModel = root.documentModelComp.createObject(root);
        }
        else{
            console.error("Unable to create document model for DocumentDataController. Error: 'documentModelComp' is invalid")
        }
    }
}
