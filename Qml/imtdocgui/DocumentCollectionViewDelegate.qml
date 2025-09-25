import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtbaseDocumentRevisionSdl 1.0

CollectionViewCommandsDelegateBase {
	id: collectionViewCommandsDelegateBase;

	property DocumentManager documentManager: null;

	/*deprecated*/property string documentTypeId;
	/*deprecated*/property string viewTypeId;

	property string documentManagerId: collectionId;

	property var documentTypeIds: [];
	/*deprecated*/property var documentViewTypeIds: [];
	property var documentViewsComp: [];
	property var documentDataControllersComp: [];
	property var documentValidatorsComp: [];

	Component.onCompleted: {
		updateDocumentManager()
	}
	
	NavigableItem {
		parentSegment: collectionViewCommandsDelegateBase.collectionId
		paths: collectionViewCommandsDelegateBase.documentTypeIds
		onActivated: {
			if (collectionViewCommandsDelegateBase.documentManager){
				if (restPath.length === 1){
					let documentTypeId = matchedPath
					let documentId = restPath[0]
					collectionViewCommandsDelegateBase.openDocumentEditor(documentId, documentTypeId)
				}
			}
		}
	}

	onDocumentManagerIdChanged: {
		updateDocumentManager()
	}
	
	function updateDocumentManager(){
		let documentManager = MainDocumentManager.getDocumentManager(documentManagerId);
		console.log("updateDocumentManager", documentManagerId, documentManager, documentTypeIds)
		if (documentManager){
			collectionViewCommandsDelegateBase.documentManager = documentManager;
			registerDocumentViews()
		}
	}

	onCollectionViewChanged: {
		if (collectionView && collectionView.dataController){
			collectionView.dataController.removed.connect(internal.onRemoved);
		}
	}


	function registerDocumentViews() {
		console.log("onDocumentManagerChanged", documentTypeIds, documentViewsComp)
		if (!documentManager){
			return
		}

		for (let i = 0; i < documentTypeIds.length; i++){
			let documentTypeId = documentTypeIds[i];
			if (documentViewsComp.length > i){
				let documentViewComp = documentViewsComp[i];
				console.log("onDocumentManagerChanged2", documentTypeId, documentViewComp)
				if (!documentManager.registerDocumentView(documentTypeId, documentViewComp)){
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

	function getHeaders(){
		return {};
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

	function createNewObject(typeId){
		if (collectionViewCommandsDelegateBase.documentManager){
			collectionViewCommandsDelegateBase.documentManager.insertNewDocument(typeId);
		}
		else{
			console.error("Unable to create new object:", typeId, ". Error: Document manager is invalid")
		}
	}

	function openDocumentEditor(objectId, typeId){
		console.debug("DocumentCollectionViewDelegate.qml openDocumentEditor", objectId, typeId)
		if (documentManager){
			documentManager.openDocument(objectId, typeId);
		}
		else{
			console.error("Unable to open document for editing", typeId, ". Error: Document manager is invalid");
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
			if (elementsModel.containsKey("id", index)){
				let itemId = elementsModel.getData("id", index);
				let typeId = elementsModel.getData("typeId", index);

				if (documentTypeIds.length === 0){
					collectionViewCommandsDelegateBase.openDocumentEditor(itemId, documentTypeId);
					return;
				}

				let typeIdindex = getDocumentTypeIdIndex(typeId);
				if (typeIdindex < 0){
					console.error('Document with type-ID: "', typeId ,'" unsupported');
				}
				else{
					collectionViewCommandsDelegateBase.openDocumentEditor(itemId, documentTypeIds[typeIdindex]);
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
			if (elementsModel.containsKey("id", index)){
				let documentId = elementsModel.getData("id", index);
				ModalDialogManager.openDialog(documentRevisionDialogComp, {"documentId": documentId});
			}
		}
	}

	function onNew(){
		if (documentTypeIds.length > 1){
			// dialog
			// collectionViewCommandsDelegateBase.createNewObject(documentTypeIds);
		}
		else{
			if (documentTypeIds.length > 0){
				collectionViewCommandsDelegateBase.createNewObject(documentTypeIds[0]);
			}
			else{
				collectionViewCommandsDelegateBase.createNewObject(documentTypeId);
			}
		}
	}

	TreeItemModel {
		id: revisionHeadersModel
		Component.onCompleted: {
			 updateHeaders()
		}

		function updateHeaders(){
			revisionHeadersModel.clear()

			let index = revisionHeadersModel.insertNewItem();
			revisionHeadersModel.setData("id", "user", index)
			revisionHeadersModel.setData("name", qsTr("User"), index)

			index = revisionHeadersModel.insertNewItem();
			revisionHeadersModel.setData("id", "revision", index)
			revisionHeadersModel.setData("name", qsTr("Revision"), index)

			index = revisionHeadersModel.insertNewItem();
			revisionHeadersModel.setData("id", "isActive", index)
			revisionHeadersModel.setData("name", qsTr("Active"), index)

			index = revisionHeadersModel.insertNewItem();
			revisionHeadersModel.setData("id", "timestamp", index)
			revisionHeadersModel.setData("name", qsTr("Time"), index)
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
			property BaseModel revisionsModel: BaseModel {};

			Component.onCompleted: {
				if (PermissionsController.checkPermission("RestoreRevision")){
					addButton(Enums.save, qsTr("Set this revision"), false)
				}

				if (PermissionsController.checkPermission("DeleteRevision")){
					addButton(Enums.no, qsTr("Delete revision"), false)
				}

				addButton(Enums.cancel, qsTr("Close"), true)
			}

			signal revisionModelReceived()
			signal receiveRevisionModelStarted()
			signal receiveRevisionModelFailed()

			onFinished: {
				if (buttonId == Enums.save){
					setRevisionRequest.send();
				}
				else if (buttonId == Enums.no){
					ModalDialogManager.openDialog(removeRevisionConfirmDialog, {});
				}
			}

			function getRevisionList(){
				receiveRevisionModelStarted()
				request.send();
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
							documentRevisionDialog.revisionsModel = m_revisions;
							documentRevisionDialog.currentRevision = m_activeRevision;

							documentRevisionDialog.revisionModelReceived()
						}
					}
				}

				function getHeaders(){
					return collectionViewCommandsDelegateBase.getHeaders();
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
							documentRevisionDialog.getRevisionList()
							ModalDialogManager.showInfoDialog(qsTr("The document revision has been successfully delete"));
						}
					}
				}

				function getHeaders(){
					return collectionViewCommandsDelegateBase.getHeaders();
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
							documentRevisionDialog.getRevisionList()
							ModalDialogManager.showInfoDialog(qsTr("The document revision has been successfully set"));
						}
					}
				}

				function getHeaders(){
					return collectionViewCommandsDelegateBase.getHeaders();
				}
			}

			Component {
				id: removeRevisionConfirmDialog;
				MessageDialog {
					width: Style.sizeHintM;
					title: qsTr("Remove revision");
					message: qsTr("Delete the selected revision ?");
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
					width: Style.sizeHintXXL;
					height: Style.sizeHintXL;

					Component.onCompleted: {
						console.log("contentItem onCompleted")
					}

					Connections {
						target: documentRevisionDialog;

						function onStarted(){
							documentRevisionDialog.getRevisionList()
						}

						function onReceiveRevisionModelStarted(){
							loading.start()
						}

						function onReceiveRevisionModelFailed(){
							loading.stop()
						}

						function onRevisionModelReceived(){
							loading.stop()
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
								anchors.leftMargin: Style.marginM;
								width: Style.iconSizeM;
								height: width;
								source: "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal);
								sourceSize.width: width;
								sourceSize.height: height;
							}

							onReused: {
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

					Item {
						id: splitView;
						anchors.fill: parent
						anchors.margins: Style.marginM

						Component.onCompleted: {
							console.log("splitView onCompleted")
						}

						Table {
							id: table;
							anchors.top: parent.top
							width: parent.width;
							height: Style.sizeHintM;
							headers: revisionHeadersModel
							elements: documentRevisionDialog.revisionsModel

							onElementsChanged: {
								table.resetSelection();
							}

							onHeadersChanged: {
								setColumnContentById("isActive", isActiveColumnDelegateComp);
							}

							onSelectionChanged: {
								documentRevisionDialog.buttons.setButtonState(Enums.save, false);
								documentRevisionDialog.buttons.setButtonState(Enums.no, false);

								let isEnabled = selection.length === 1;
								if (isEnabled){
									if (selection.length > 0){
										let index = selection[0]
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
						}

						Rectangle {
							id: descriptionItem;
							anchors.top: table.bottom
							width: parent.width;
							height: Style.sizeHintXXS;
							clip: true
							color: Style.backgroundColor2

							Text {
								id: descriptionTitle;
								anchors.top: parent.top;
								anchors.topMargin: Style.marginM;
								color: Style.buttonTextColor;
								font.family: Style.fontFamilyBold;
								font.pixelSize: Style.fontSizeM;
								font.bold: true;
								text: qsTr("Description");
							}

							Rectangle {
								id: background;
								anchors.top: descriptionTitle.bottom;
								anchors.topMargin: Style.marginM;
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
								secondSize: Style.marginXS;
								targetItem: flickable;
							}

							Flickable {
								id: flickable;
								anchors.centerIn: background;
								width: background.width - 2 * Style.marginM;
								height: background.height - 2 * Style.marginM;
								contentHeight: descriptionTextField.height;
								clip: true;
								boundsBehavior: Flickable.StopAtBounds;

								Text {
									id: descriptionTextField;
									width: parent.width;
									font.family: Style.fontFamily;
									font.pixelSize: Style.fontSizeM;
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

