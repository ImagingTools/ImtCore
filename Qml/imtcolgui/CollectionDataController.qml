import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

QtObject {
	id: root;
	
	property string collectionId;

	function insertNewObject(typeId, name, description, objectId, object){
		return false
	}
	
	function removeObject(objectId){
		return false
	}
	
	function setObjectName(objectId, name){
		return false
	}

	function setObjectDescription(objectId, description){
		return false
	}
	
	function setObject(typeId, name, description, objectId, object){
		return false
	}
	
	function getObject(objectId){
		return false
	}
	
	function getCollectionHeaders(){
		return false
	}
	
	function getElements(){
		return false
	}
}

