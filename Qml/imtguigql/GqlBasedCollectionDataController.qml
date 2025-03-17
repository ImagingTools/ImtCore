import QtQuick 2.0
import Acf 1.0
import imtcolgui 1.0
import imtbaseImtCollectionSdl 1.0

CollectionDataController {
	id: root;
	
	property ObjectInfo objectInfo: ObjectInfo {}
	property SetObjectInput setObjectInput: SetObjectInput {}
	
	property GqlSdlRequestSender getObjectRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_getObject;
		sdlObjectComp: Component {
			ObjectResponse {
				onFinished: {
					root.objectReceived(m_objectId, m_object);
				}
			}
		}
	}
	
	property GqlSdlRequestSender insertNewObjectRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_insertNewObject;
		requestType: 1;
		sdlObjectComp: Component {
			CollectionResponse {
				onFinished: {
					root.objectReceived(m_objectId, m_object);
				}
			}
		}
	}
	
	property GqlSdlRequestSender removeElementRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_removeElement;
		requestType: 1;
		sdlObjectComp: Component {
			CollectionResponse {
				onFinished: {
					if (m_objectId !== ""){
						root.objectRemoved()
					}
					else{
						root.objectRemoveFailed()
					}
				}
			}
		}
	}
	
	property GqlSdlRequestSender setObjectRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_setObject;
		requestType: 1;
		sdlObjectComp: Component {
			CollectionResponse {
				onFinished: {
					if (m_objectId !== ""){
						root.objectUpdated()
					}
					else{
						root.objectUpdateFailed()
					}
				}
			}
		}
	}
	
	signal objectReceived(string objectId, string object)
	signal objectReceiveFailed()
	
	signal objectRemoved()
	signal objectRemoveFailed()
	
	signal objectInserted()
	signal objectInsertFailed()
	
	signal objectUpdated()
	signal objectUpdateFailed()
	
	function insertNewObject(typeId, name, description, objectId, object){
		setObjectInput.m_typeId = typeId
		setObjectInput.m_name = name
		setObjectInput.m_description = description
		setObjectInput.m_objectId = objectId
		setObjectInput.m_object = object
		setObjectInput.m_collectionId = collectionId
		insertNewObjectRequest.send(setObjectInput)
		return true
	}
	
	function removeObject(objectId){
		objectInfo.m_objectId = objectId
		objectInfo.m_collectionId = collectionId
		removeElementRequest.send(objectInfo)
		return true
	}
	
	function setObjectName(objectId, name){
		return true
	}

	function setObjectDescription(objectId, description){
		return true
	}
	
	function setObject(typeId, name, description, objectId, object){
		setObjectInput.m_typeId = typeId
		setObjectInput.m_name = name
		setObjectInput.m_description = description
		setObjectInput.m_objectId = objectId
		setObjectInput.m_object = object
		setObjectInput.m_collectionId = collectionId
		setObjectRequest.send(setObjectInput)
		return true
	}
	
	function getObject(objectId){
		objectInfo.m_objectId = objectId
		objectInfo.m_collectionId = collectionId
		getObjectRequest.send(objectInfo)
		return true
	}
}
