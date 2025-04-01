import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtgui 1.0
import imtguigql 1.0
import imtbaseImtCollectionSdl 1.0

DocumentWorkspacePageView {
	id: root;
	
	documentManagerView: documentManagerView;
	
	onStartItemSourceCompChanged: {
		if (startItemSourceComp){
			documentManagerView.addInitialItem(startItemSourceComp, pageName);
		}
	}
	
	SingleDocumentWorkspaceView {
		id: documentManagerView;
		anchors.fill: parent;
		documentManager: root.documentManager;
		
		function getViewName(id, typeId){
			objectVisualStatusInput.m_objectId = id
			objectVisualStatusInput.m_typeId = typeId
			
			getVisualStatusInfoRequest.send(objectVisualStatusInput)
			
			return ""
		}
	}
	
	ObjectVisualStatusInput {
		id: objectVisualStatusInput;
	}
	
	GqlSdlRequestSender {
		id: getVisualStatusInfoRequest;
		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_getObjectVisualStatus;
		sdlObjectComp: Component {
			VisualStatus {
				onFinished: {
					let name = m_text;
					if (name === ""){
						name = root.documentManager.defaultDocumentName;
					}
					
					documentManagerView.setViewName(m_objectId, name)
				}
			}
		}
		
		function onError(message, type){}
	}
}
