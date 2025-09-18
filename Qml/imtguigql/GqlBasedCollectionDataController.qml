import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtguigql 1.0
// import prolifeAccountsSdl 1.0
import imtbaseImtCollectionSdl 1.0

CollectionDataController {
	id: root

	function getObject(objectId){
		inputId.m_id = objectId
		
		// getRequestSender.send(inputId)
	}
	
	function insertNewObject(objectId, objectData){
		// accountDataInput.m_id = objectId
		// accountDataInput.m_item = objectData
		
		// addRequestSender.send(accountDataInput)
	}
	
	function updateObject(objectId, newObjectData){
		// accountDataInput.m_id = objectId
		// accountDataInput.m_item = newObjectData
		
		updateRequestSender.send(accountDataInput)
	}
	
	function getElements(count, offset, filter){
		collectionViewParams.m_offset = offset
		collectionViewParams.m_count = count
		collectionViewParams.m_filterModel = filter
		
		// getListRequestSender.send(collectionViewParams)
	}
	
	function removeObject(objectId){
		inputId.m_id = objectId
		
		// removeRequestSender.send(inputId)
	}
	
	function setObjectName(objectId, name){
		renameInput.m_id = objectId
		renameInput.m_name = name
		
		// setNameRequestSender.send(renameInput)
	}
	
	function setObjectDescription(objectId, description){
		setDescriptionInput.m_id = objectId
		setDescriptionInput.m_description = description
		
		// setDescriptionRequestSender.send(setDescriptionInput)
	}
	
	// property RenameInput renameInput: RenameInput {}
	// property SetDescriptionInput setDescriptionInput: SetDescriptionInput {}
	
	// property InputId inputId: InputId {}
	// property AccountDataInput accountDataInput: AccountDataInput {}
	// property CollectionViewParams collectionViewParams: CollectionViewParams {
	// 	m_offset: 0
	// 	m_count: -1
	// }

	// property GqlSdlRequestSender getRequestSender: GqlSdlRequestSender {
	// 	gqlCommandId: ProlifeAccountsSdlCommandIds.s_accountItem
	// 	sdlObjectComp: Component {
	// 		AccountData {
	// 			onFinished: {
	// 				root.objectReceived(this)
	// 			}
	// 		}
	// 	}
	// }
	
	// property GqlSdlRequestSender addRequestSender: GqlSdlRequestSender {
	// 	gqlCommandId: ProlifeAccountsSdlCommandIds.s_accountAdd
	// 	sdlObjectComp: Component {
	// 		AddedNotificationPayload {
	// 			onFinished: {
	// 				root.objectAdded(m_id)
	// 			}
	// 		}
	// 	}
	// }
	
	// property GqlSdlRequestSender updateRequestSender: GqlSdlRequestSender {
	// 	gqlCommandId: ProlifeAccountsSdlCommandIds.s_accountUpdate
	// 	sdlObjectComp: Component {
	// 		UpdatedNotificationPayload {
	// 			onFinished: {
	// 				root.objectUpdated(m_id)
	// 			}
	// 		}
	// 	}
	// }

	// property GqlSdlRequestSender getListRequestSender: GqlSdlRequestSender {
	// 	gqlCommandId: ProlifeAccountsSdlCommandIds.s_accountsList
	// 	sdlObjectComp: Component {
	// 		AccountsListPayload {
	// 			onFinished: {
	// 				root.listObjectsReceived(m_items, m_notification)
	// 			}
	// 		}
	// 	}
	// }
	
	// property GqlSdlRequestSender removeRequestSender: GqlSdlRequestSender {
	// 	gqlCommandId: ProlifeAccountsSdlCommandIds.s_removeAccount
	// 	sdlObjectComp: Component {
	// 		RemovedNotificationPayload {
	// 			onFinished: {
	// 				root.objectRemoved(m_id)
	// 			}
	// 		}
	// 	}
	// }
	
	// property GqlSdlRequestSender setNameRequestSender: GqlSdlRequestSender {
	// 	gqlCommandId: ProlifeAccountsSdlCommandIds.s_setAccountName
	// 	sdlObjectComp: Component {
	// 		RenamedNotificationPayload {
	// 			onFinished: {
	// 				root.objectRenamed(m_id, m_name)
	// 			}
	// 		}
	// 	}
	// }
	
	// property GqlSdlRequestSender setDescriptionRequestSender: GqlSdlRequestSender {
	// 	gqlCommandId: ProlifeAccountsSdlCommandIds.s_setAccountDescription
	// 	sdlObjectComp: Component {
	// 		SetDescriptionNotificationPayload {
	// 			onFinished: {
	// 				root.objectDescriptionSetted(m_id, m_description)
	// 			}
	// 		}
	// 	}
	// }
}

