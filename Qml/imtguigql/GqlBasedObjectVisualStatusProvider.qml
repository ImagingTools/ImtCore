import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtgui 1.0
import imtbaseImtCollectionSdl 1.0

ObjectVisualStatusProvider {
	id: root

	property string collectionId
	property ObjectVisualStatusInput objectVisualStatusInput: ObjectVisualStatusInput {
		m_collectionId: root.collectionId
	}

	function getVisualStatus(id, typeId){
		if (collectionId === ""){
			console.error("Unable to get visual status for object: '"+id+"', type-ID: '"+typeId+"'. Error: Collection-ID is empty")
			return
		}

		objectVisualStatusInput.m_objectId = id
		objectVisualStatusInput.m_typeId = typeId
		getVisualStatusInfoRequest.send(objectVisualStatusInput)
	}
	
	function getHeaders(){
		return {}
	}
	
	property GqlSdlRequestSender getVisualStatusInfoRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_getObjectVisualStatus
		sdlObjectComp: Component {
			VisualStatus {
				onFinished: {
					root.visualStatusReceived(m_objectId, m_icon, m_text, m_description)
				}
			}
		}

		function onError(message, type){
			root.visualStatusReceiveFailed(message)
		}

		function getHeaders(){
			return root.getHeaders()
		}
	}
}

