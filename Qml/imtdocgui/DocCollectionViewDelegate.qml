import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtbaseDocumentRevisionSdl 1.0

CollectionViewCommandsDelegateBase {
	id: commandsDelegate
	
	property DocumentManagerBase documentManager: null
	property string documentManagerId: collectionId

	property var documentTypeIds: []
	property var documentTypeNames: []
	property var documentViewTypeIds: []
	property var documentEditorsComp: []
	property var documentDataControllersComp: []

	Component.onCompleted: {
		if (documentManager){
			registerDocumentTypes()
		}
	}

	onDocumentManagerIdChanged: {
		if (documentManagerId !== ""){
			documentManager = MainDocumentManager.getDocumentManager(documentManagerId)
		}
	}

	onDocumentManagerChanged: {
		if (documentManager){
			registerDocumentTypes()
		}
	}

	function registerDocumentTypes(){
		if (!documentManager){
			console.error("Unable to register document types: '"+documentTypeIds+"'. Error: Document manager is invalid")
			return
		}
		
		if (!(documentTypeIds.length === documentTypeNames.length &&
			  documentTypeNames.length && documentViewTypeIds.length && 
			  documentViewTypeIds.length === documentEditorsComp.length &&
			  documentEditorsComp.length === documentDataControllersComp.length)){
			console.error("Unable to register document types. Error: inconsistent or missing document type configuration arrays")
			return
		}
		
		for (let i = 0; i < documentTypeIds.length; ++i){
			let documentTypeId = documentTypeIds[i]
			let documentViewTypeId = documentViewTypeIds[i]
			let documentEditorComp = documentEditorsComp[i]
			let documentDataControllerComp = documentDataControllersComp[i]
			
			documentManager.registerDocumentViewData(documentTypeId, documentViewTypeId, documentEditorComp, documentDataControllerComp)
		}
	}

	function onEdit(){
		if (!collectionView){
			console.error("Unable to edit element. Error: Collection view is invalid")
			return
		}

		if (!documentManager){
			console.error("Unable to remove elements. Error: Document manager is invalid")
			return
		}
		
		let elementsModel = collectionView.table.elements
		if (!elementsModel){
			console.error("Unable to edit document. Error: Elements for collection view is invalid")
			return
		}
		
		let indexes = collectionView.table.getSelectedIndexes()
		if (indexes.length >= 0){
			let itemId = elementsModel.getData("id", indexes[0]);
			let typeId = elementsModel.getData("typeId", indexes[0]);
			
			documentManager.openDocument(typeId, itemId)
		}
	}
	
	function onNew(){
		if (!documentManager){
			console.error("Unable to create object. Error: Document manager is invalid")
			return
		}

		if (documentTypeIds.length === 0){
			console.error("Unable to create new document. Type-ID is empty")
			return
		}

		if (documentTypeIds.length > 1){
			ModalDialogManager.openDialog(selectTypeIdDialogComp)
		}
		else{
			let documentTypeId = documentTypeIds[0]
			documentManager.createDocument(documentTypeId)
		}
	}
	
	Component {
		id: selectTypeIdDialogComp
		
		Dialog {
			id: selectTypeIdDialog
			title: qsTr("Select Document Type")
			canMove: false
			width: 300
			
			property string selectedDocumentTypeId
			
			Component.onCompleted: {
				addButton(Enums.ok, qsTr("OK"), true)
				addButton(Enums.cancel, qsTr("Cancel"), true)
			}
			
			onFinished: {
				if (buttonId === Enums.ok){
					if (selectedDocumentTypeId !== ""){
						commandsDelegate.documentManager.createDocument(selectedDocumentTypeId)
					}
					else{
						console.error("Unable to create document with type-ID: '" +selectedDocumentTypeId+"'")
					}
				}
			}
			
			contentComp: Component {
				Item {
					width: selectTypeIdDialog.width
					height: content.height
					Column {
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
							currentIndex: 0
							
							onCurrentIndexChanged: {
								if (currentIndex >= 0){
									selectTypeIdDialog.selectedDocumentTypeId = documentTypeCbModel.getData("id", currentIndex)
								}
								else{
									selectTypeIdDialog.selectedDocumentTypeId = ""
								}
							}
							
							TreeItemModel {
								id: documentTypeCbModel
								Component.onCompleted: {
									for (let i = 0; i < commandsDelegate.documentTypeIds.length; ++i){
										let documentTypeId = commandsDelegate.documentTypeIds[i]
										let documentTypeName = commandsDelegate.documentTypeNames[i]
										
										let index = insertNewItem()
										setData("id", documentTypeId, index)
										setData("name", documentTypeName, index)
									}
									
									documentTypeCb.model = documentTypeCbModel
								}
							}
						}
					}
				}
			}
		}
	}
}

