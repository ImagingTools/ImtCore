import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtguigql 1.0
import controlsgalleryContactInfosSdl 1.0
import controlsgalleryContactInfoCollectionDocumentServiceSdl 1.0
import imtbaseCollectionDocumentServiceSdl 1.0
import imtbaseUndoManagerSdl 1.0

// Demonstrates that DocumentService is transport-agnostic: the exact same
// ContactInfoEditor view is driven by two independent DocumentService
// instances that differ only in which backend is injected — one talks to
// the server over GraphQL, the other keeps everything in memory on the
// client. Neither the editor nor DocumentService itself knows or cares
// which one it is.
Rectangle {
	id: testPage;

	anchors.fill: parent;
	clip: true;
	color: Style.baseColor

	Component {
		id: contactInfoEditorComp

		ContactInfoEditor {
			id: contactInfoEditor
			commandsControllerComp: Component {
				GqlBasedCommandsController {
					typeId: "ContactInfo"
				}
			}
		}
	}

	Row {
		anchors.fill: parent
		spacing: 1

		// --- Left pane: server-backed contacts (GraphQL) ---------------------
		Column {
			width: (parent.width - parent.spacing) / 2
			height: parent.height

			Text {
				width: parent.width
				height: Style.controlHeightL
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				font.pixelSize: Style.fontSizeL
				color: Style.titleColor
				text: qsTr("Server-backed (GraphQL)")
			}

			DocumentTabsWorkspace {
				width: parent.width
				height: parent.height - Style.controlHeightL
				documentManager: DocumentService {
					id: gqlContactService
					backend: GqlDocumentServiceBackend {
						collectionId: "ContactInfos"
					}
					Component.onCompleted: {
						registerDocumentViewData("ContactInfo", "ContactInfoEditor", contactInfoEditorComp, gqlContactControllerComp)
						createDocument("ContactInfo")
					}

					Component {
						id: gqlContactControllerComp

						DocumentRepresentationController {
							id: root

							representationModel: ContactInfoData {}

							function updateRepresentationFromDocument(){
								startUpdateRepresentation(documentId, representationModel)

								documentIdInput.m_id = documentId
								getContactInfoRequest.send(documentIdInput)
							}

							function updateDocumentFromRepresentation(){
								startUpdateDocument(documentId)

								updateContactInfoInput.m_documentId = documentId
								updateContactInfoInput.m_contactInfo = representationModel

								updateContactInfoRequest.send(updateContactInfoInput)
							}

							property DocumentId documentIdInput: DocumentId {}
							property GqlSdlRequestSender getContactInfoRequest: GqlSdlRequestSender {
								gqlCommandId: ControlsgalleryContactInfoCollectionDocumentServiceSdlCommandIds.s_getContactInfoRepresentation
								sdlObjectComp: Component {
									ContactInfoData {
										onFinished: {
											root.representationModel.copyFrom(this)
											root.representationUpdated(root.documentId, root.representationModel)
										}
									}
								}

								function onError(message, type){
									root.updateRepresentationFailed(root.documentId, message)
								}
							}

							property UpdateContactInfoInput updateContactInfoInput: UpdateContactInfoInput {}
							property GqlSdlRequestSender updateContactInfoRequest: GqlSdlRequestSender {
								gqlCommandId: ControlsgalleryContactInfoCollectionDocumentServiceSdlCommandIds.s_updateContactInfoFromRepresentation
								requestType: 1
								sdlObjectComp: Component {
									DocumentOperationStatus {
										onFinished: {
											if (m_status === "Success"){
												root.documentUpdated(root.documentId)
											}
										}
									}
								}

								function onError(message, type){
									root.updateDocumentFailed(root.documentId, message)
								}
							}
						}
					}
				}
			}
		}

		Rectangle {
			width: 1
			height: parent.height
			color: Style.borderColor
		}

		// --- Right pane: purely local contacts (no network at all) -----------
		Column {
			width: (parent.width - parent.spacing) / 2
			height: parent.height

			Text {
				width: parent.width
				height: Style.controlHeightL
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				font.pixelSize: Style.fontSizeL
				color: Style.titleColor
				text: qsTr("Local (in-memory, no server)")
			}

			DocumentTabsWorkspace {
				width: parent.width
				height: parent.height - Style.controlHeightL
				documentManager: DocumentService {
					id: localContactService
					backend: LocalDocumentServiceBackend {
						id: localContactBackend
						Component.onCompleted: {
							registerDocumentType("ContactInfo", contactInfoDataComp)
						}

						Component {
							id: contactInfoDataComp
							ContactInfoData {}
						}
					}
					Component.onCompleted: {
						registerDocumentViewData("ContactInfo", "ContactInfoEditor", contactInfoEditorComp, localContactControllerComp)
						createDocument("ContactInfo")
					}

					Component {
						id: localContactControllerComp

						InProcessDocumentRepresentationController {
							backend: localContactBackend
						}
					}
				}
			}
		}
	}
}
