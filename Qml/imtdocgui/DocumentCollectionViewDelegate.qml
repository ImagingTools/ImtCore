import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtbaseDocumentRevisionSdl 1.0

CollectionViewCommandsDelegateBase {
	id: collectionViewCommandsDelegateBase;

	property DocumentManager documentManager: null;

	property string documentTypeId; // default document type-ID
	property string viewTypeId; // default document view type-ID

	property string documentManagerId: collectionId;

	property var documentTypeIds: [];
	property var documentViewTypeIds: [];
	property var documentViewsComp: [];
	property var documentDataControllersComp: [];
	property var documentValidatorsComp: [];

	onCollectionViewChanged: {
		if (collectionView && collectionView.dataController){
			collectionView.dataController.removed.connect(internal.onRemoved);
		}
	}

	onDocumentManagerChanged: {
		console.log("onDocumentManagerChanged", documentManager);
		for (let i = 0; i < documentTypeIds.length; i++){
			let documentTypeId = documentTypeIds[i];
			console.log("documentTypeId", documentTypeId);


			if (documentViewsComp.length > i && documentViewTypeIds.length > i){
				let viewTypeId = documentViewTypeIds[i];
				let documentViewComp = documentViewsComp[i];
				console.log("viewTypeId", viewTypeId, documentViewComp);

				if (!documentManager.registerDocumentView(documentTypeId, viewTypeId, documentViewComp)){
					console.error("Unable to register view for document type ID: ", documentTypeId)
				}
			}

			if (documentDataControllersComp.length > i){
				let documentDataControllerComp = documentDataControllersComp[i];
				documentManager.registerDocumentDataController(documentTypeId, documentDataControllerComp);
			}

			if (documentValidatorsComp.length > i){
				let documentValidatorComp = documentValidatorsComp[i];
				documentManager.registerDocumentValidator(documentTypeId, documentValidatorComp);
			}
		}
	}

	function getDocumentTypeIdIndex(typeId){
		for (let i = 0; i < documentTypeIds.length; i++){
			if (typeId === documentTypeIds[i]){
				return i;
			}
		}

		return -1;
	}

	onCollectionIdChanged: {
		let documentManager = MainDocumentManager.getDocumentManager(collectionViewCommandsDelegateBase.documentManagerId);
		if (documentManager){
			collectionViewCommandsDelegateBase.documentManager = documentManager;
		}
	}

	QtObject {
		id: internal;

		function onRemoved(objectId){
			if (collectionViewCommandsDelegateBase.documentManager){
				let index = collectionViewCommandsDelegateBase.documentManager.getDocumentIndexByDocumentId(objectId);
				if (index >= 0){
					collectionViewCommandsDelegateBase.documentManager.closeDocument(objectId, true);
				}
			}
		}
	}

	function createNewObject(typeId, viewTypeId){
		if (collectionViewCommandsDelegateBase.documentManager){
			collectionViewCommandsDelegateBase.documentManager.insertNewDocument(typeId, viewTypeId);
		}
		else{
			console.error("Unable to create new object:", typeId, viewTypeId, ". Error: Document manager is invalid")
		}
	}

	function openDocumentEditor(objectId, typeId, viewTypeId){
		if (collectionViewCommandsDelegateBase.documentManager){
			collectionViewCommandsDelegateBase.documentManager.openDocument(objectId, typeId, viewTypeId);
		}
		else{
			console.error("Unable to open document for editing", typeId, viewTypeId, ". Error: Document manager is invalid");
		}
	}

	function onEdit(){
		let elementsModel = collectionViewCommandsDelegateBase.collectionView.table.elements;
		if (!elementsModel){
			console.error("Unable to edit document. Error: Elements for collection view is invalid");
			return;
		}

		let indexes = collectionViewCommandsDelegateBase.collectionView.table.getSelectedIndexes();
		for (let i = 0; i < indexes.length; i++){
			let index = indexes[i];
			if (elementsModel.containsKey("Id", index)){
				let itemId = elementsModel.getData("Id", index);
				let typeId = elementsModel.getData("TypeId", index);

				if (documentTypeIds.length === 0){
					collectionViewCommandsDelegateBase.openDocumentEditor(itemId, documentTypeId, viewTypeId);
					return;
				}

				let typeIdindex = getDocumentTypeIdIndex(typeId);
				if (index < 0){
					console.error('Document with type-ID: "', typeId ,'" unsupported');
				}
				else{
					collectionViewCommandsDelegateBase.openDocumentEditor(itemId, documentTypeIds[typeIdindex], documentViewTypeIds[typeIdindex]);
				}
			}
		}
	}

	function onRevision(){
		let elementsModel = collectionViewCommandsDelegateBase.collectionView.table.elements;
		if (!elementsModel){
			console.error("Unable to get revisions for document. Error: Elements for collection view is invalid");
			return;
		}

		let indexes = collectionViewCommandsDelegateBase.collectionView.table.getSelectedIndexes();
		if (indexes.length > 0){
			let index = indexes[0];
			if (elementsModel.containsKey("Id", index)){
				let documentId = elementsModel.getData("Id", index);
				ModalDialogManager.openDialog(documentRevisionDialogComp, {"documentId": documentId});
			}
		}
	}

	function onNew(){
		if (documentTypeIds.length > 1){
			// dialog
			// collectionViewCommandsDelegateBase.createNewObject(documentTypeIds, viewTypeId);
		}
		else{
			if (documentTypeIds.length > 0){
				collectionViewCommandsDelegateBase.createNewObject(documentTypeIds[0], documentViewTypeIds[0]);
			}
			else{
				collectionViewCommandsDelegateBase.createNewObject(documentTypeId, viewTypeId);
			}
		}
	}

	Component {
		id: documentRevisionDialogComp;

		Dialog {
			id: documentRevisionDialog;
			title: qsTr("Revision Manager");
			canMove: false;

			notClosingButtons: Enums.no | Enums.save;

			property string documentId;
			property int selectedRevision: -1;
			property int currentRevision: -1;
			property BaseModel revisionsModel;

			Component.onCompleted: {
				if (PermissionsController.checkPermission("RestoreRevision")){
					buttonsModel.append({Id: Enums.save, Name:qsTr("Set this revision"), Enabled: false})
				}

				if (PermissionsController.checkPermission("DeleteRevision")){
					buttonsModel.append({Id: Enums.no, Name:qsTr("Delete revision"), Enabled: false})
				}

				buttonsModel.append({Id: Enums.cancel, Name:qsTr("Close"), Enabled: true})
			}

			onFinished: {
				if (buttonId == Enums.save){
					setRevisionRequest.send();
				}
				else if (buttonId == Enums.no){
					ModalDialogManager.openDialog(removeRevisionConfirmDialog, {});
				}
			}

			GqlSdlRequestSender {
				id: request;
				gqlCommandId: ImtbaseDocumentRevisionSdlCommandIds.s_getRevisionInfoList;
				inputObjectComp: Component {
					GetRevisionInfoListInput {
						m_documentId: documentRevisionDialog.documentId;
						m_collectionId: collectionViewCommandsDelegateBase.collectionId;
					}
				}

				sdlObjectComp: Component { RevisionInfoList {
						onFinished: {
							console.log("onFinished", m_revisions.toJson());
							documentRevisionDialog.revisionsModel = m_revisions;
							documentRevisionDialog.currentRevision = m_activeRevision;
						}
					}
				}

				onStateChanged: {
					// loading.visible = (state === "Loading");
				}
			}

			GqlSdlRequestSender {
				id: deleteRevisionRequest;
				gqlCommandId: ImtbaseDocumentRevisionSdlCommandIds.s_deleteRevision;
				inputObjectComp: Component {
					DeleteRevisionInput {
						m_objectId: documentRevisionDialog.documentId;
						m_revision: documentRevisionDialog.selectedRevision;
						m_collectionId: collectionViewCommandsDelegateBase.collectionId;
					}
				}

				sdlObjectComp: Component { DeleteRevisionResponse {
						onFinished: {
							request.send();
							ModalDialogManager.showInfoDialog(qsTr("The document revision has been successfully delete"));
						}
					}
				}
			}

			GqlSdlRequestSender {
				id: setRevisionRequest;
				requestType: 1;
				gqlCommandId: ImtbaseDocumentRevisionSdlCommandIds.s_restoreRevision;
				inputObjectComp: Component {
					RestoreRevisionInput {
						m_objectId: documentRevisionDialog.documentId;
						m_revision: documentRevisionDialog.selectedRevision;
						m_collectionId: collectionViewCommandsDelegateBase.collectionId;
					}
				}

				sdlObjectComp: Component { RestoreRevisionResponse {
						onFinished: {
							request.send();
							ModalDialogManager.showInfoDialog(qsTr("The document revision has been successfully set"));
						}
					}
				}
			}

			Component {
				id: removeRevisionConfirmDialog;
				MessageDialog {
					width: 400;
					title: qsTr("Remove revision");
					message: qsTr("Delete the selected revision?");
					onFinished: {
						if (buttonId == Enums.yes){
							deleteRevisionRequest.send()
						}
					}
				}
			}

			contentComp: Component {
				Item {
					id: contentItem;
					width: 700;
					height: 600;

					Connections {
						target: documentRevisionDialog;

						function onStarted(){
							request.send();
						}
					}

					Component {
						id: isActiveColumnDelegateComp;
						TableCellDelegateBase {
							id: cellDelegate

							Image {
								id: image;
								anchors.verticalCenter: parent.verticalCenter;
								anchors.left: parent.left;
								anchors.leftMargin: Style.size_mainMargin;
								width: 20;
								height: width;
								source: "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal);
								sourceSize.width: width;
								sourceSize.height: height;
							}

							onRowIndexChanged: {
								if (!rowDelegate){
									return
								}

								if (rowIndex >= 0){
									let isActive = table.elements.get(rowIndex).item.m_isActive;
									image.visible = isActive;
								}
							}
						}
					}

					SplitView {
						id: splitView;
						anchors.left: parent.left;
						anchors.leftMargin: Style.size_mainMargin;
						anchors.right: parent.right;
						anchors.rightMargin: Style.size_mainMargin;
						height: parent.height;
						orientation: Qt.Vertical

						Table {
							id: table;
							width: parent.width;
							height: 400;
							elements: documentRevisionDialog.revisionsModel;

							onElementsChanged: {
								table.resetSelection();
							}

							onHeadersChanged: {
								setColumnContentById("m_isActive", isActiveColumnDelegateComp);
							}

							onSelectionChanged: {
								documentRevisionDialog.buttons.setButtonState(Enums.save, false);
								documentRevisionDialog.buttons.setButtonState(Enums.no, false);

								let isEnabled = selection.length === 1;
								if (isEnabled){
									if (selection.length > 0){
										let index = selection[0];
										let selectedRevision = table.elements.get(index).item.m_revision;
										if (selectedRevision !== documentRevisionDialog.currentRevision){
											documentRevisionDialog.selectedRevision = selectedRevision;
											documentRevisionDialog.buttons.setButtonState(Enums.save, true);
											documentRevisionDialog.buttons.setButtonState(Enums.no, true);
										}

										descriptionTextField.text = table.elements.get(index).item.m_description;
									}
								}
							}

							TreeItemModel {
								id: headersModel;
								Component.onCompleted: {
									let index = headersModel.insertNewItem();
									headersModel.setData("Id", "m_user", index)
									headersModel.setData("Name", qsTr("User"), index)

									index = headersModel.insertNewItem();
									headersModel.setData("Id", "m_revision", index)
									headersModel.setData("Name", qsTr("Revision"), index)

									index = headersModel.insertNewItem();
									headersModel.setData("Id", "m_isActive", index)
									headersModel.setData("Name", qsTr("Active"), index)

									index = headersModel.insertNewItem();
									headersModel.setData("Id", "m_timestamp", index)
									headersModel.setData("Name", qsTr("Time"), index)

									table.headers = headersModel;
								}
							}
						}

						Item {
							id: descriptionItem;
							width: parent.width;
							height: 200;

							Text {
								id: descriptionTitle;
								anchors.top: parent.top;
								anchors.topMargin: Style.size_mainMargin;
								color: Style.buttonText;
								font.family: Style.fontFamilyBold;
								font.pixelSize: Style.fontSize_common;
								font.bold: true;
								text: qsTr("Description");
							}

							Rectangle {
								id: background;
								anchors.top: descriptionTitle.bottom;
								anchors.topMargin: Style.size_mainMargin;
								anchors.bottom: parent.bottom;
								width: parent.width;
								border.width: 1;
								border.color: Style.borderColor2;
								radius: 3;
							}

							CustomScrollbar {
								id: scrollbar;
								z: parent.z + 1;
								anchors.right: parent.right;
								anchors.top: parent.top;
								anchors.bottom: parent.bottom;
								secondSize: Style.size_smallMargin;
								targetItem: flickable;
							}

							Flickable {
								id: flickable;
								anchors.centerIn: background;
								width: background.width - 2 * Style.size_mainMargin;
								height: background.height - 2 * Style.size_mainMargin;
								contentHeight: descriptionTextField.height;
								clip: true;
								boundsBehavior: Flickable.StopAtBounds;

								Text {
									id: descriptionTextField;
									width: parent.width;
									font.family: Style.fontFamily;
									font.pixelSize: Style.fontSize_common;
									color: Style.textColor;
									wrapMode: Text.WordWrap;
								}
							}
						}
					}

					Loading {
						id: loading;
						anchors.fill: parent;
						visible: false
					}
				}
			}
		}
	}
}
