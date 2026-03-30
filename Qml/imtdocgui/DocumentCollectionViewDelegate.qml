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
	
	property bool isSingleDocumentMode: false

	Component.onCompleted: {
		updateDocumentManager()
	}

	onDocumentManagerIdChanged: {
		updateDocumentManager()
	}
	
	function updateDocumentManager(){
		let documentManager = MainDocumentManager.getDocumentManager(documentManagerId);
		if (documentManager){
			collectionViewCommandsDelegateBase.documentManager = documentManager;
			registerDocumentViews()
		}
	}

	Connections {
		target: collectionViewCommandsDelegateBase.collectionView && collectionViewCommandsDelegateBase.collectionView.dataController ? collectionViewCommandsDelegateBase.collectionView.dataController : undefined
		function onRemoved(objectIds){
			if (collectionViewCommandsDelegateBase.documentManager){
				for (let i = 0; i < objectIds.length; ++i){
					let index = collectionViewCommandsDelegateBase.documentManager.getDocumentIndexByDocumentId(objectIds[i])
					if (index >= 0){
						collectionViewCommandsDelegateBase.documentManager.closeDocument(objectIds[i], true)
					}
				}
			}
		}
	}

	function registerDocumentViews() {
		if (!documentManager){
			return
		}

		for (let i = 0; i < documentTypeIds.length; i++){
			let documentTypeId = documentTypeIds[i];
			if (documentViewsComp.length > i){
				let documentViewComp = documentViewsComp[i];
				if (!documentManager.registerDocumentView(documentTypeId, documentViewComp)){
					console.error("Unable to register view for document type ID: '" +documentTypeId+"'")
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

	function createNewObject(typeId){
		if (documentManager){
			documentManager.insertNewDocument(typeId);
		}
		else{
			console.error("Unable to create new object with type-ID: '"+ typeId + "' . Error: Document manager is invalid")
		}
	}

	function openDocumentEditor(objectId, typeId){
		if (!documentManager){
			console.log("Unable to open document editor for type-ID: '" + typeId + "'. Error: Document manager is invalid")
			return
		}

		if (isSingleDocumentMode){
			let openedDocumentIds = documentManager.getOpenedDocumentIds()
			for (let i = 0; i < openedDocumentIds.length; ++i){
				documentManager.closeDocument(openedDocumentIds[i], true)
			}
		}

		documentManager.openDocument(objectId, typeId);
		NavigationController.push(documentManagerId+"/"+typeId+"/"+objectId)
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

	Component {
		id: documentRevisionDialogComp;

		DocumentRevisionDialog {
			collectionId: collectionViewCommandsDelegateBase.collectionId
			function getHeaders(){
				return collectionViewCommandsDelegateBase.getHeaders();
			}
		}
	}
}

