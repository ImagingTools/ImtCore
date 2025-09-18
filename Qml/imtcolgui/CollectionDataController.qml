import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {
	id: root

	signal collectionHeadersReceived(var headersModel)
	signal collectionHeadersReceiveFailed(var headersModel)

	signal objectReceived(var object)
	signal objectReceiveFailed(string message)

	signal objectAdded(string id)
	signal objectAddFailed(string message)

	signal objectUpdated(string id)
	signal objectUpdateFailed(string message)

	signal objectsRemoved(var ids)
	signal removeObjectsFailed(string message)

	signal objectsRestored(var ids)
	signal restoreObjectsFailed(string message)

	signal listObjectsReceived(var listObjects, var notification)
	signal listObjectsReceiveFailed(string message)

	signal objectRenamed(string id, string newName)
	signal objectRenameFailed(string message)

	signal objectDescriptionSetted(string id, string description)
	signal objectSetDescriptionFailed(string message)

	function getCollectionHeaders(){
		console.warn("getHeaders() should be implemented in a subclass")
		collectionHeadersReceived(null)
	}

	function getObject(objectId){
		console.warn("getObject() should be implemented in a subclass")
		objectReceived(null)
	}

	function insertNewObject(objectId, objectData){
		console.warn("insertNewObject() should be implemented in a subclass")
		objectAdded(objectId)
	}

	function updateObject(objectId, newObjectData){
		console.warn("updateObject() should be implemented in a subclass")
		objectUpdated(objectId)
	}

	function getElements(count, offset, filter){
		console.warn("getElements() should be implemented in a subclass")
		listObjectsReceived(null, null)
	}

	function removeObjects(objectIds){
		console.warn("removeObjects() should be implemented in a subclass")
		objectsRemoved(objectIds)
	}

	function removeObjectSet(filter){
		console.warn("removeObjectSet() should be implemented in a subclass")
		objectsRemoved([])
	}

	function restoreObjects(objectIds){
		console.warn("restoreObjects() should be implemented in a subclass")
		objectsRestored(objectIds)
	}

	function restoreObjectSet(filter){
		console.warn("restoreObjectSet() should be implemented in a subclass")
		objectsRestored([])
	}

	function setObjectName(objectId, name){
		console.warn("setObjectName() should be implemented in a subclass")
		objectRenamed(objectId, name)
	}

	function setObjectDescription(objectId, description){
		console.warn("setObjectDescription() should be implemented in a subclass")
		objectDescriptionSetted(objectId, description)
	}
}

