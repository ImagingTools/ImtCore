import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtguigql 1.0
import controlsgalleryContactInfosSdl 1.0
import controlsgalleryContactInfoCollectionDocumentManagerSdl 1.0
import imtbaseCollectionDocumentManagerSdl 1.0
import imtbaseUndoManagerSdl 1.0

Rectangle {
	id: testPage;

	anchors.fill: parent;
	clip: true;
	color: Style.baseColor


	MultiDocumentCollectionView {
		anchors.fill: parent;
		// documentManager: GqlBasedCollectionDocumentManager {
		// 	collectionId: "ContactInfos"
		// 	Component.onCompleted: {
		// 		registerDocumentViewData("ContactInfo", "ContactInfoEditor", contactInfoEditorComp, contactInfoDataControllerFactory)
		// 		createDocument("ContactInfo")
		// 	}

		// 	Component {
		// 		id: contactInfoEditorComp
				
		// 		ContactInfoEditor {
		// 			id: contactInfoEditor
		// 			commandsControllerComp: Component {
		// 				GqlBasedCommandsController {
		// 					typeId: "ContactInfo"
		// 				}
		// 			}
		// 		}
		// 	}
			
		// 	Component {
		// 		id: contactInfoDataControllerFactory

		// 		DocumentRepresentationController {
		// 			id: root

		// 			representationModel: ContactInfoData {}

		// 			function updateRepresentationFromDocument(){
		// 				startUpdateRepresentation(documentId, representationModel)

		// 				documentIdInput.m_id = documentId
		// 				getContactInfoRequest.send(documentIdInput)
		// 			}

		// 			function updateDocumentFromRepresentation(){
		// 				startUpdateDocument(documentId)

		// 				updateContactInfoInput.m_documentId = documentId
		// 				updateContactInfoInput.m_contactInfo = representationModel

		// 				updateContactInfoRequest.send(updateContactInfoInput)
		// 			}

		// 			property DocumentId documentIdInput: DocumentId {}
		// 			property GqlSdlRequestSender getContactInfoRequest: GqlSdlRequestSender {
		// 				gqlCommandId: ControlsgalleryContactInfoCollectionDocumentManagerSdlCommandIds.s_getContactInfoRepresentation
		// 				sdlObjectComp: Component {
		// 					ContactInfoData {
		// 						onFinished: {
		// 							root.representationModel.copyFrom(this)
		// 							root.representationUpdated(root.documentId, root.representationModel)
		// 						}
		// 					}
		// 				}

		// 				function onError(message, type){
		// 					root.updateRepresentationFailed(root.documentId, message)
		// 				}
		// 			}

		// 			property UpdateContactInfoInput updateContactInfoInput: UpdateContactInfoInput {}
		// 			property GqlSdlRequestSender updateContactInfoRequest: GqlSdlRequestSender {
		// 				gqlCommandId: ControlsgalleryContactInfoCollectionDocumentManagerSdlCommandIds.s_updateContactInfoFromRepresentation
		// 				requestType: 1
		// 				sdlObjectComp: Component {
		// 					DocumentOperationStatus {
		// 						onFinished: {
		// 							if (m_status === "Success"){
		// 								root.documentUpdated(root.documentId)
		// 							}
		// 						}
		// 					}
		// 				}

		// 				function onError(message, type){
		// 					root.updateDocumentFailed(root.documentId, message)
		// 				}
		// 			}
		// 		}
		// 	}
		// }
	}
}
