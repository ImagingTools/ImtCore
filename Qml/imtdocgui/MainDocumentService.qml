pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

// Registry of document services keyed by page-ID, shared by every page that
// hosts a document workspace (both the new DocumentService family and the
// older client-only DocumentService family). Because it fans out to
// managers of either family, every cross-manager operation below is
// duck-typed defensively instead of assuming a specific manager's method
// set.
QtObject {
	id: root

	property var documentManagers: ({})

	signal documentOpened(string typeId, string documentId, string documentTypeId)
	signal tryRegisterDocumentService(string typeId, var callback)

	function clear(){
		root.documentManagers = {}
	}

	function getDocumentService(typeId){
		return root.documentManagers[typeId]
	}

	function getDocumentDataByView(viewRef){
		let typeIds = Object.keys(root.documentManagers)
		for (let i = 0; i < typeIds.length; i++){
			let manager = root.documentManagers[typeIds[i]]
			if (manager && typeof manager.getDocumentDataByView === "function"){
				let document = manager.getDocumentDataByView(viewRef)
				if (document != null){
					return document
				}
			}
		}

		return null
	}

	function unregisterDocumentService(documentManager){
		let typeIds = Object.keys(root.documentManagers)
		for (let i = 0; i < typeIds.length; i++){
			let manager = root.documentManagers[typeIds[i]]
			if (manager === documentManager){
				delete root.documentManagers[typeIds[i]]
				return true
			}
		}

		return false
	}

	function registerDocumentService(typeId, documentManager){
		if (!typeId || typeId === ""){
			console.error("Unable to register document manager. Type-ID is invalid!")
			return false
		}

		if (!documentManager){
			console.error("Unable to register document manager. Reference on the document manager is invalid!")
			return false
		}

		root.documentManagers[typeId] = documentManager

		return true
	}

	// typeId: page/routing key the manager was registered under.
	// documentId: object-ID to open within that manager's collection.
	// documentTypeId: the object's document type-ID.
	function openDocument(typeId, documentId, documentTypeId){
		if ((!typeId || typeId === "") || (!documentId || documentId === "") || (!documentTypeId || documentTypeId === "")){
			console.error("Unable to open document, please check input parameters")
			return
		}

		let documentManager = root.documentManagers[typeId]
		if (!documentManager){
			root.tryRegisterDocumentService(typeId, function(result){
				if (result){
					root.openDocument(typeId, documentId, documentTypeId)
				}
				else{
					console.error("Unable to open document. Document manager with type-ID: '", typeId, "' is invalid!")
				}
			})
			return
		}

		documentOpened(typeId, documentId, documentTypeId)

		documentManager.openDocument(documentTypeId, documentId)
	}

	function saveDirtyDocuments(){
		let typeIds = Object.keys(root.documentManagers)
		for (let i = 0; i < typeIds.length; i++){
			let documentManager = root.documentManagers[typeIds[i]]
			if (!documentManager || typeof documentManager.getOpenedDocumentIds !== "function"){
				continue
			}

			let documentIds = documentManager.getOpenedDocumentIds()
			for (let j = 0; j < documentIds.length; j++){
				if (documentManager.documentIsDirty(documentIds[j])){
					documentManager.saveDocument(documentIds[j])
				}
			}
		}
	}

	function dirtyDocumentsExists(){
		let typeIds = Object.keys(root.documentManagers)
		for (let i = 0; i < typeIds.length; i++){
			let documentManager = root.documentManagers[typeIds[i]]
			if (!documentManager || typeof documentManager.getOpenedDocumentIds !== "function"){
				continue
			}

			let documentIds = documentManager.getOpenedDocumentIds()
			for (let j = 0; j < documentIds.length; j++){
				if (documentManager.documentIsDirty(documentIds[j])){
					return true
				}
			}
		}

		return false
	}

	// Closes every open document across every registered manager. Each
	// manager handles its own dirty-document confirmation internally, so
	// this simply fires one closeDocument() per open document rather than
	// sequencing them with a callback chain.
	function closeAllDocuments(){
		let typeIds = Object.keys(root.documentManagers)
		for (let i = 0; i < typeIds.length; i++){
			let documentManager = root.documentManagers[typeIds[i]]
			if (!documentManager || typeof documentManager.getOpenedDocumentIds !== "function"){
				continue
			}

			let documentIds = documentManager.getOpenedDocumentIds()
			for (let j = 0; j < documentIds.length; j++){
				documentManager.closeDocument(documentIds[j])
			}
		}
	}
}
