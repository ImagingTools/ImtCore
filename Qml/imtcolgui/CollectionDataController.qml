import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {
	id: root

	property string collectionId

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

	signal listObjectsReceived(var listObjects)
	signal listObjectsReceiveFailed(string message)

	signal objectRenamed(string id, string newName)
	signal objectRenameFailed(string message)

	signal objectDescriptionSetted(string id, string description)
	signal objectSetDescriptionFailed(string message)

	signal subCollectionCreated(var subCollection)
	signal subCollectionCreateFailed(string message)

	function getCollectionHeaders(){
		console.warn("getHeaders() should be implemented in a subclass")
		collectionHeadersReceived(null)
	}

	function getObjectData(objectId){
		console.warn("getObjectData() should be implemented in a subclass")
		objectReceived(null)
	}

	function insertNewObject(typeId, name, description, objectData, objectId){
		console.warn("insertNewObject() should be implemented in a subclass")
		objectAdded(objectId)
	}

	function setObjectData(objectId, newObjectData){
		console.warn("setObjectData() should be implemented in a subclass")
		objectUpdated(objectId)
	}

	function getElements(count, offset, filter){
		console.warn("getElements() should be implemented in a subclass")
		listObjectsReceived(null)
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

	function createSubCollection(offset, count, selectionParams){
		console.warn("createSubCollection() should be implemented in a subclass")
		subCollectionCreated(null)
	}
}

