import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtbaseImtCollectionSdl 1.0

CollectionDataController {
	id: root

	// HTTP headers !!!
	function getHeaders(){
		return {};
	}

	// Collection Headers
	property GetCollectionHeadersInput getCollectionHeadersInput : GetCollectionHeadersInput {}
	property GqlSdlRequestSender getCollectionHeadersRequest : GqlSdlRequestSender {
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_getCollectionHeaders
		sdlObjectComp: Component {
			GetCollectionHeadersPayload {
				onFinished: {
					root.collectionHeadersReceived(m_headers)
				}
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}

	function getCollectionHeaders(){
		getCollectionHeadersInput.m_collectionId = collectionId
		getCollectionHeadersRequest.send(getCollectionHeadersInput)
	}

	// Get Object Data
	property GetObjectDataInput getObjectDataInput : GetObjectDataInput {}
	property GqlSdlRequestSender getObjectDataRequest : GqlSdlRequestSender {
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_getObjectData
		sdlObjectComp: Component {
			GetObjectDataPayload {
				onFinished: {
					root.objectReceived(m_objectData)
				}
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}

	function getObjectData(objectId){
		getObjectDataInput.m_collectionId = collectionId
		getObjectDataInput.m_objectId = objectId
		getObjectDataRequest.send(getObjectDataInput)
	}

	// Insert New Object
	property InsertNewObjectInput insertNewObjectInput : InsertNewObjectInput {}
	property GqlSdlRequestSender insertNewObjectRequest : GqlSdlRequestSender {
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_insertNewObject
		requestType: 1
		sdlObjectComp: Component {
			InsertNewObjectPayload {
				onFinished: {
					root.objectAdded(m_objectId)
				}
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}

	function insertNewObject(typeId, name, description, objectData, objectId){
		insertNewObjectInput.m_collectionId = collectionId

		insertNewObjectInput.m_typeId = typeId
		insertNewObjectInput.m_name = name
		insertNewObjectInput.m_description = description

		if (objectData){
			insertNewObjectInput.m_objectData = objectData
		}

		if (objectId){
			insertNewObjectInput.m_proposedObjectId = objectId
		}

		insertNewObjectRequest.send(insertNewObjectInput)
	}

	// Set Object Data
	property SetObjectDataInput setObjectDataInput : SetObjectDataInput {}
	property GqlSdlRequestSender setObjectDataRequest : GqlSdlRequestSender {
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_setObjectData
		requestType: 1
		sdlObjectComp: Component {
			SetObjectDataPayload {
				onFinished: {
					root.objectUpdated()
				}
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}

	function setObjectData(objectId, newObjectData){
		setObjectDataInput.m_collectionId = collectionId
		setObjectDataInput.m_objectId = objectId
		setObjectDataInput.m_objectData = newObjectData
		setObjectDataRequest.send(setObjectDataInput)
	}

	// Get Elements
	function getElements(count, offset, filter){
		console.warn("getElements() should be implemented in a subclass")
		listObjectsReceived(null, null)
	}

	// Remove Objects
	property RemoveElementsInput removeElementsInput : RemoveElementsInput {}
	property GqlSdlRequestSender removeElementsRequest : GqlSdlRequestSender{
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_removeElements
		requestType: 1
		sdlObjectComp: Component {
			RemoveElementsPayload {
				onFinished: {
					root.objectsRemoved()
				}
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}

	function removeObjects(objectIds){
		removeElementsInput.m_collectionId = collectionId
		removeElementsInput.m_elementIds = objectIds
		removeElementsRequest.send(removeElementsInput)
	}

	// Remove Object Set
	property RemoveElementSetInput removeElementSetInput : RemoveElementSetInput {}
	property GqlSdlRequestSender removeElementSetRequest : GqlSdlRequestSender{
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_removeElementSet
		requestType: 1
		sdlObjectComp: Component {
			RemoveElementSetPayload {
				onFinished: {
					root.objectsRemoved()
				}
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}

	function removeObjectSet(selectionParams){
		removeElementSetInput.m_collectionId = collectionId
		removeElementSetInput.m_selectionParams = selectionParams
		removeElementSetRequest.send(removeElementSetInput)
	}

	// Restore Objects
	property RestoreObjectsInput restoreObjectsInput : RestoreObjectsInput {}
	property GqlSdlRequestSender restoreObjectsRequest : GqlSdlRequestSender{
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_restoreObjects
		requestType: 1
		sdlObjectComp: Component {
			RestoreObjectsPayload {
				onFinished: {
					root.objectsRestored()
				}
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}

	function restoreObjects(objectIds){
		restoreObjectsInput.m_collectionId = collectionId
		restoreObjectsInput.m_objectIds = objectIds
		restoreObjectsRequest.send(restoreObjectsInput)
	}

	// Restore Object Set
	property RestoreObjectSetInput restoreObjectSetInput : RestoreObjectSetInput {}
	property GqlSdlRequestSender restoreObjectSetRequest : GqlSdlRequestSender{
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_restoreObjectSet
		requestType: 1
		sdlObjectComp: Component {
			RestoreObjectSetPayload {
				onFinished: {
					root.objectsRestored()
				}
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}

	function restoreObjectSet(selectionParams){
		restoreObjectSetInput.m_collectionId = collectionId
		restoreObjectSetInput.m_selectionParams = selectionParams
		restoreObjectSetRequest.send(restoreObjectSetInput)
	}

	// Set Object Name
	property SetObjectNameInput setObjectNameInput : SetObjectNameInput {}
	property GqlSdlRequestSender setObjectNameRequest : GqlSdlRequestSender{
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_setObjectName
		requestType: 1
		sdlObjectComp: Component {
			SetObjectNamePayload {
				onFinished: {
					if (m_success){
						root.objectRenamed(m_objectId, m_name)
					}
					else{
						root.objectRenameFailed()
					}
				}
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}

	function setObjectName(objectId, name){
		setObjectNameInput.m_collectionId = collectionId
		setObjectNameInput.m_objectId = objectId
		setObjectNameInput.m_name = name
		setObjectNameRequest.send(setObjectNameInput)
	}

	// Set Object Description
	property SetObjectDescriptionInput setObjectDescriptionInput : SetObjectDescriptionInput {}
	property GqlSdlRequestSender setObjectDescriptionRequest : GqlSdlRequestSender{
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_setObjectDescription
		requestType: 1
		sdlObjectComp: Component {
			SetObjectDescriptionPayload {
				onFinished: {
					if (m_success){
						root.objectDescriptionSetted(m_objectId, m_description)
					}
					else{
						root.objectSetDescriptionFailed()
					}
				}
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}

	// Create Sub Collection
	property CreateSubCollectionInput createSubCollectionInput : CreateSubCollectionInput {}
	property GqlSdlRequestSender createSubCollectionRequest : GqlSdlRequestSender{
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_createSubCollection
		sdlObjectComp: Component {
			CreateSubCollectionPayload {
				onFinished: {
					root.subCollectionCreated(m_items)
				}
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}

	function createSubCollection(offset, count, selectionParams){
		createSubCollectionInput.m_collectionId = collectionId
		createSubCollectionInput.m_offset = offset
		createSubCollectionInput.m_count = count
		createSubCollectionInput.m_selectionParams = selectionParams
		createSubCollectionRequest.send(createSubCollectionInput)
	}
}

