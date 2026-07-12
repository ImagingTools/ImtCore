import QtQuick 2.12

// Single place that instantiates an editor view + representation controller
// + DocumentViewBinder for one (document, viewTypeId) pair. Replaces the
// triplicated inline creation code that used to live in
// MultiDocumentCollectionView, SingleDocumentCollectionView and
// SingleDocumentWorkspaceContentView.
QtObject {
	id: root

	required property var documentService

	function resolveViewTypeId(document, viewTypeId){
		if (viewTypeId){
			return viewTypeId
		}

		let viewTypeIds = documentService.editors.getSupportedViewTypeIds(document.typeId)
		return viewTypeIds.length > 0 ? viewTypeIds[0] : ""
	}

	// Creates the representation controller for an editor view that already
	// exists (e.g. instantiated by a StackView's own addPage()), wires the
	// DocumentViewBase contract properties on it, and registers the
	// resulting (view, controller) pair with the document service. Returns
	// the controller, or null on failure (already logged).
	function attachController(document, viewTypeId, view){
		if (!document || !view){
			console.error("DocumentEditorHost: invalid document or view")
			return null
		}

		let controllerComp = documentService.editors.getRepresentationControllerFactory(document.typeId, viewTypeId)
		if (!controllerComp){
			console.error("DocumentEditorHost: no representation-controller factory for '" + document.typeId + "/" + viewTypeId + "'")
			return null
		}

		let controller = controllerComp.createObject(view, {documentId: document.documentId})
		if (!controller){
			console.error("DocumentEditorHost: failed to instantiate representation controller for '" + document.typeId + "/" + viewTypeId + "'")
			return null
		}

		if (view.documentManager !== undefined){
			view.documentManager = documentService
		}
		if (view.documentId !== undefined){
			view.documentId = document.documentId
		}
		if (view.documentTypeId !== undefined){
			view.documentTypeId = document.typeId
		}

		documentService.registerDocumentView(document.documentId, viewTypeId, view, controller)

		return controller
	}

	// Creates and fully wires an editor view for `document` inside
	// `parentItem`. `viewTypeId` may be empty to pick the first registered
	// view for the document's type. Returns the created view Item, or null
	// on failure (already logged).
	function createEditor(document, viewTypeId, parentItem){
		if (!document || !parentItem){
			console.error("DocumentEditorHost: invalid document or parent item")
			return null
		}

		let resolvedViewTypeId = resolveViewTypeId(document, viewTypeId)
		if (!resolvedViewTypeId){
			console.error("DocumentEditorHost: no registered views for type '" + document.typeId + "'")
			return null
		}

		let editorComp = documentService.editors.getEditorFactory(document.typeId, resolvedViewTypeId)
		if (!editorComp){
			console.error("DocumentEditorHost: no editor factory for '" + document.typeId + "/" + resolvedViewTypeId + "'")
			return null
		}

		let view = editorComp.createObject(parentItem)
		if (!view){
			console.error("DocumentEditorHost: failed to instantiate editor for '" + document.typeId + "/" + resolvedViewTypeId + "'")
			return null
		}
		view.anchors.fill = parentItem

		if (!attachController(document, resolvedViewTypeId, view)){
			view.destroy()
			return null
		}

		return view
	}
}
