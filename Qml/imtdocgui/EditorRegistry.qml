import QtQuick 2.12

// Registry of editor/representation-controller factories for document
// type-IDs. Extracted out of the document service so that "which editor
// renders which document type" is a standalone concern (SRP), reusable by
// both DocCollectionViewDelegate and SingleDocumentTypeRegistrar.
QtObject {
	id: root

	// typeId -> [{viewTypeId, editorComp, controllerComp}]
	property var __types: ({})

	signal viewRegistered(string documentTypeId, string viewTypeId)

	function register(documentTypeId, viewTypeId, editorComp, controllerComp){
		if (!documentTypeId || !viewTypeId || !editorComp || !controllerComp){
			console.error("EditorRegistry: invalid registration arguments for type '" + documentTypeId + "'")
			return false
		}

		let entry = {viewTypeId: viewTypeId, editorComp: editorComp, controllerComp: controllerComp}

		if (documentTypeId in __types){
			let registeredViews = __types[documentTypeId]
			for (let i = 0; i < registeredViews.length; ++i){
				if (registeredViews[i].viewTypeId === viewTypeId){
					console.error("EditorRegistry: view '" + viewTypeId + "' already registered for type '" + documentTypeId + "'")
					return false
				}
			}
			registeredViews.push(entry)
		}
		else{
			__types[documentTypeId] = [entry]
		}

		viewRegistered(documentTypeId, viewTypeId)
		return true
	}

	function getSupportedTypeIds(){
		return Object.keys(__types)
	}

	function getSupportedViewTypeIds(documentTypeId){
		if (!(documentTypeId in __types)){
			return []
		}

		return __types[documentTypeId].map(entry => entry.viewTypeId)
	}

	function getEditorFactory(documentTypeId, viewTypeId){
		let entry = __find(documentTypeId, viewTypeId)
		return entry ? entry.editorComp : null
	}

	function getRepresentationControllerFactory(documentTypeId, viewTypeId){
		let entry = __find(documentTypeId, viewTypeId)
		return entry ? entry.controllerComp : null
	}

	function getViewTypeIdByEditorFactory(documentTypeId, editorComp){
		if (!(documentTypeId in __types)){
			return ""
		}

		let entries = __types[documentTypeId]
		for (let i = 0; i < entries.length; ++i){
			if (entries[i].editorComp === editorComp){
				return entries[i].viewTypeId
			}
		}

		return ""
	}

	function __find(documentTypeId, viewTypeId){
		if (!(documentTypeId in __types)){
			return null
		}

		let entries = __types[documentTypeId]
		if (!viewTypeId){
			return entries.length > 0 ? entries[0] : null
		}

		for (let i = 0; i < entries.length; ++i){
			if (entries[i].viewTypeId === viewTypeId){
				return entries[i]
			}
		}

		return null
	}
}
