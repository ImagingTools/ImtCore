import QtQuick 2.0
import Acf 1.0
import imtguigql 1.0
import imtbaseImtCollectionSdl 1.0

ObjectVisualStatusProvider {
	id: root

	property ObjectVisualStatusInput objectVisualStatusInput: ObjectVisualStatusInput {}
	
	function getVisualStatus(id, typeId){
		objectVisualStatusInput.m_objectId = id
		objectVisualStatusInput.m_typeId = typeId
		
		getVisualStatusInfoRequest.send(objectVisualStatusInput)
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
	}
}

