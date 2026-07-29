import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {
	id: root;
	
	property var documentModel;
	
	property string documentId;
	property string documentName;
	property string documentDescription;
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
		hasRemoteChanges = false;
		modelChanged();
	}
	
	function getDocumentId(){
		return documentId;
	}
	
	function getDocumentName(){
		return documentName;
	}
	
	function getDocumentTypeId(){
		return typeId;
	}
	
	function getDocumentModel(){
	}
	
	function getDocumentDescription(){
		return documentDescription;
	}
	
	function updateDocumentModel(){
		modelChanged();
	}
	
	function insertDocument(){
		saved(documentId, documentName)
	}
	
	function saveDocument(){
		saved(documentId, documentName)
	}
	
	function createDocumentModel(){
		if (root.documentModelComp){
			documentModel = root.documentModelComp.createObject(root);
		}
		else{
			console.error("Unable to create document model for DocumentDataController. Error: 'documentModelComp' is invalid")
		}
	}
}
