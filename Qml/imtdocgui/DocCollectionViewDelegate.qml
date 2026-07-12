import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtbaseDocumentRevisionSdl 1.0

CollectionViewCommandsDelegateBase{
	id: commandsDelegate
	
	property DocumentService documentManager: null
	property string documentManagerId: collectionId

	// When true, opening/creating a document closes every other document
	// already open on this documentManager first — for editors backed by a
	// single-document breadcrumb workspace rather than a tab strip, where
	// only one document makes sense open at a time.
	property bool isSingleDocumentMode: false

	property var documentConfigs: ({})

	QtObject {
		id: internal
		property var registeredDocumentViews: []
	}

	// If a row this delegate has open for editing gets removed from the
	// collection (e.g. by another user), close its editor instead of
	// leaving it open on data that no longer exists server-side.
	Connections {
		target: commandsDelegate.collectionView && commandsDelegate.collectionView.dataController
					? commandsDelegate.collectionView.dataController : null

		function onRemoved(objectIds){
			if (!commandsDelegate.documentManager){
				return
			}

			for (let i = 0; i < objectIds.length; ++i){
				let documentId = commandsDelegate.documentManager.getDocumentIdByObjectId(objectIds[i])
				if (documentId !== ""){
					// force: the row backing this document was just deleted
					// server-side, so there is nothing left to save changes
					// against - prompting to save would only error out.
					commandsDelegate.documentManager.closeDocument(documentId, true)
				}
			}
		}
	}

	function registerDocumentType(typeId, typeName){
		if (!typeId){
			console.error("Cannot register document type with empty ID")
			return
		}

		if (!documentConfigs[typeId]){
			documentConfigs[typeId] = {
				typeName: typeName || typeId,
				views: []
			}
		}
		else{
			if (typeName){
				documentConfigs[typeId].typeName = typeName
			}
		}
	}

	function addDocumentView(typeId, viewTypeId, editorComp, controllerComp){
		if (!documentConfigs[typeId]){
			console.error("Cannot add view. Document type '" + typeId + "' is not registered")
			return
		}

		if (!viewTypeId || !editorComp || !controllerComp){
			console.error("Cannot add view for '" + typeId + "'. Missing arguments")
			return
		}

		documentConfigs[typeId].views.push({
											viewTypeId: viewTypeId,
											editorComp: editorComp,
											controllerComp: controllerComp,
											registered: false
											})

		if (documentManager){
			registerDocumentTypes()
		}
	}
	
	Component.onCompleted:{
		if (documentManager){
			registerDocumentTypes()
		}
	}

	// Connections (not onXChanged:) so that a consumer instantiating this
	// delegate can still safely declare its own onDocumentManagerIdChanged/
	// onDocumentManagerChanged handlers (e.g. to register a data controller
	// once documentManager resolves) without silently replacing this logic:
	// on<Signal>: is a single property slot per object and a later
	// declaration at the instantiation site would otherwise override it.
	Connections {
		target: commandsDelegate

		function onDocumentManagerIdChanged(){
			if (commandsDelegate.documentManagerId !== ""){
				commandsDelegate.documentManager = MainDocumentService.getDocumentService(commandsDelegate.documentManagerId)
			}
		}

		function onDocumentManagerChanged(){
			if (commandsDelegate.documentManager){
				commandsDelegate.registerDocumentTypes()
			}
		}
	}

	function registerDocumentTypes(){
		if (!documentManager){
			console.error("Unable to register document types. Document manager is invalid.")
			return
		}

		for (let typeId in documentConfigs){
			let config = documentConfigs[typeId]
			if (!config.views || config.views.length === 0){
				console.warn("Document type '" + typeId + "' has no views")
				continue
			}

			for (let i = 0; i < config.views.length; ++i){
				let v = config.views[i]
				if (!v.registered){
					documentManager.registerDocumentViewData(
								typeId, v.viewTypeId, v.editorComp, v.controllerComp
								)
					v.registered = true
				}
			}
		}
	}

	function getRegisteredDocumentTypeIds(){
		return Object.keys(documentConfigs)
	}

	function getDocumentTypeName(typeId) {
		let config = documentConfigs[typeId]
		return config ? config.typeName : ""
	}

	function onEdit(){
		if (!collectionView){
			console.error("Unable to edit element. Error: Collection view is invalid")
			return
		}

		if (!documentManager){
			console.error("Unable to edit elements. Error: Document manager is invalid")
			return
		}

		let elementsModel = collectionView.table.elements
		if (!elementsModel){
			console.error("Unable to edit document. Error: Elements for collection view is invalid")
			return
		}

		let indexes = collectionView.table.getSelectedIndexes()
		for (let i = 0; i < indexes.length; ++i){
			if (!elementsModel.containsKey("id", indexes[i])){
				console.error("Unable to edit element. Field: 'id' does not exists in the table model")
				return
			}

			if (!elementsModel.containsKey("typeId", indexes[i])){
				console.error("Unable to edit element. Field: 'typeId' does not exists in the table model")
				return
			}

			let itemId = elementsModel.getData("id", indexes[i]);
			let typeId = elementsModel.getData("typeId", indexes[i]);

			if (isSingleDocumentMode){
				__closeAllOpenDocuments()
			}

			documentManager.openDocument(typeId, itemId)
		}
	}

	function __closeAllOpenDocuments(){
		let openedDocumentIds = documentManager.getOpenedDocumentIds()
		for (let i = 0; i < openedDocumentIds.length; ++i){
			documentManager.closeDocument(openedDocumentIds[i])
		}
	}

	function onNew(){
		if (!documentManager){
			console.error("Unable to create object. Error: Document manager is invalid")
			return
		}

		let documentTypeIds = getRegisteredDocumentTypeIds()
		if (documentTypeIds.length === 0){
			console.error("Unable to create new document. Type-ID is empty")
			return
		}

		if (isSingleDocumentMode){
			__closeAllOpenDocuments()
		}

		if (documentTypeIds.length > 1){
			ModalDialogManager.openDialog(selectTypeIdDialogComp,{"documentTypeIds": documentTypeIds})
		}
		else{
			let documentTypeId = documentTypeIds[0]
			documentManager.createDocument(documentTypeId)
		}
	}

	function onRevision(){
		if (!collectionView){
			console.error("Unable to get revision documents. Error: Collection view is invalid")
			return
		}

		let elementsModel = collectionView.table.elements;
		if (!elementsModel){
			console.error("Unable to get revisions for document. Error: Elements for collection view is invalid");
			return;
		}

		let indexes = collectionView.table.getSelectedIndexes();
		if (indexes.length > 0){
			let index = indexes[0];
			if (elementsModel.containsKey("id", index)){
				let documentId = elementsModel.getData("id", index);
				ModalDialogManager.openDialog(documentRevisionDialogComp, {"documentId": documentId});
			}
		}
	}

	Component{
		id: selectTypeIdDialogComp
		
		Dialog{
			id: selectTypeIdDialog
			title: qsTr("Select Document Type")
			canMove: false
			width: 300
			
			property string selectedDocumentTypeId
			property var documentTypeIds: []
			onDocumentTypeIdsChanged: {
				if (contentItem){
					contentItem.createDocumentTypesModel(documentTypeIds)
				}
			}

			Component.onCompleted:{
				addButton(Enums.ok, qsTr("OK"), true)
				addButton(Enums.cancel, qsTr("Cancel"), true)
			}
			
			onFinished:{
				if (buttonId === Enums.ok){
					if (selectedDocumentTypeId !== ""){
						commandsDelegate.documentManager.createDocument(selectedDocumentTypeId)
					}
					else{
						console.error("Unable to create document with type-ID: '" +selectedDocumentTypeId+"'")
					}
				}
			}
			
			contentComp: Component{
				Item{
					width: selectTypeIdDialog.width
					height: content.height

					function createDocumentTypesModel(documentTypeIds){
						documentTypeCbModel.clear()

						for (let i = 0; i < documentTypeIds.length; ++i){
							let documentTypeId = documentTypeIds[i]
							let documentTypeName = commandsDelegate.getDocumentTypeName(documentTypeId)
							if (documentTypeName === ""){
								documentTypeName = documentTypeId
							}

							let index = documentTypeCbModel.insertNewItem()
							documentTypeCbModel.setData("id", documentTypeId, index)
							documentTypeCbModel.setData("name", documentTypeName, index)
						}
						
						documentTypeCb.model = documentTypeCbModel
						documentTypeCb.currentIndex = 0
					}

					TreeItemModel{
						id: documentTypeCbModel
					}

					Column{
						id: content
						anchors.top: parent.top
						anchors.topMargin: Style.marginL
						anchors.horizontalCenter: parent.horizontalCenter
						width: parent.width - 2 * Style.marginL
						spacing: Style.marginM
						
						BaseText {
							width: parent.width
							text: qsTr("Please select a document type")
						}
						
						ComboBox {
							id: documentTypeCb
							width: parent.width

							onCurrentIndexChanged:{
								if (currentIndex >= 0){
									selectTypeIdDialog.selectedDocumentTypeId = documentTypeCbModel.getData("id", currentIndex)
								}
								else{
									selectTypeIdDialog.selectedDocumentTypeId = ""
								}
							}
						}
					}
				}
			}
		}
	}

	Component {
		id: documentRevisionDialogComp;
		DocumentRevisionDialog {
			collectionId: commandsDelegate.collectionId
		}
	}
}

