import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtCollectionSdl 1.0
import imtbaseCollectionDocumentServiceSdl 1.0
import imtdocgui 1.0

// Universal one-document content host built on top of DocumentServiceBase.
//
// Renders exactly one document view (no TabView, no CollectionView), and
// keeps the loading-indicator / view-instance lifecycle in sync with the
// service signals. Designed to be reusable by any DocumentServiceBase
// implementation (including GqlBasedCollectionDocumentService) so that
// single-document workspaces share the exact same server-side pipeline
// as multi-document collection workspaces.
Item {
	id: contentView

	// Target document service. The view binds to its signals for loading,
	// updates and lifecycle.
	property DocumentServiceBase documentManager: null

	// Id of the document this view is responsible for. Empty until the
	// service emits documentOpened/documentCreated for an object that this
	// view is interested in (set externally by the page wrapper).
	property string documentId: ""

	// Document type id. Used to look up the registered editor/representation
	// controller components on documentManager.
	property string documentTypeId: ""

	// When true, the standard Loading overlay is visible during loading.
	property bool showStandardLoading: true

	// Emitted when the underlying view loads/unloads. Useful for parent
	// containers that need to bridge focus / commands.
	signal viewLoaded(string documentId, var view)
	signal viewUnloaded(string documentId)

	// Loading-control signals (mirroring MultiDocumentCollectionView so that
	// nested views can subscribe to a single source).
	signal startLoading(string documentId)
	signal stopLoading(string documentId)

	property QtObject _internal: QtObject {
		property var hostedItem: null
		property var representationController: null
	}

	onDocumentIdChanged: {
		_rebuild()
	}

	onDocumentTypeIdChanged: {
		_rebuild()
	}

	onDocumentManagerChanged: {
		_rebuild()
	}

	function _rebuild(){
		_destroyHosted()

		if (!documentManager || documentId === "" || documentTypeId === ""){
			return
		}

		let viewTypeIds = documentManager.getSupportedDocumentViewTypeIds(documentTypeId)
		if (!viewTypeIds || viewTypeIds.length === 0){
			console.error("SingleDocumentWorkspaceContentView: no registered views for type '" + documentTypeId + "'")
			return
		}

		let viewTypeId = viewTypeIds[0]
		let viewComp = documentManager.getDocumentEditorFactory(documentTypeId, viewTypeId)
		if (!viewComp){
			console.error("SingleDocumentWorkspaceContentView: editor factory missing for type '" + documentTypeId + "/" + viewTypeId + "'")
			return
		}

		let item = viewComp.createObject(viewContainer)
		if (!item){
			console.error("SingleDocumentWorkspaceContentView: failed to instantiate editor for '" + documentTypeId + "/" + viewTypeId + "'")
			return
		}

		item.anchors.fill = viewContainer
		_internal.hostedItem = item

		if (item.objectName === "DocumentViewBase"){
			if (item.documentManagerView !== undefined){
				item.documentManagerView = contentView
			}
			if (item.documentManager !== undefined){
				item.documentManager = contentView.documentManager
			}
			if (item.documentId !== undefined){
				item.documentId = contentView.documentId
			}
			if (item.documentTypeId !== undefined){
				item.documentTypeId = contentView.documentTypeId
			}
		}

		documentManager.onViewInstanceCreated(documentId, item, viewTypeId)
		viewLoaded(documentId, item)
	}

	function _destroyHosted(){
		if (_internal.hostedItem){
			let oldId = ""
			if (_internal.hostedItem.documentId !== undefined){
				oldId = _internal.hostedItem.documentId
			}
			_internal.hostedItem.destroy()
			_internal.hostedItem = null
			viewUnloaded(oldId)
		}
	}

	Connections {
		id: connections
		target: contentView.documentManager

		function onDocumentGuiUpdated(documentId, representation){
			if (documentId !== contentView.documentId){
				return
			}
			if (!contentView.documentManager.documentIsLoading(documentId)){
				contentView.stopLoading(documentId)
			}
		}

		function onDocumentDataLoaded(documentId){
			if (documentId !== contentView.documentId){
				return
			}
			contentView.stopLoading(documentId)
		}

		function onStartUpdateRepresentation(documentId, representation){
			if (documentId !== contentView.documentId){
				return
			}
			contentView.startLoading(documentId)
		}

		function onUpdateRepresentationFailed(documentId, message){
			if (documentId !== contentView.documentId){
				return
			}
			if (!contentView.documentManager.documentIsLoading(documentId)){
				contentView.stopLoading(documentId)
			}

			if(message !==""){
				PopupManager.addErrorMessage(message, true)
			}
		}

		function onUpdateDocumentFailed(documentId, message){
			if (documentId !== contentView.documentId){
				return
			}
			if (!contentView.documentManager.documentIsLoading(documentId)){
				contentView.stopLoading(documentId)
			}

			if(message !==""){
				PopupManager.addErrorMessage(message, true)
			}
		}

		function onStartOpenDocument(documentId, typeId){
			if (documentId !== contentView.documentId){
				return
			}
			contentView.startLoading(documentId)
		}

		function onStartCloseDocument(documentId){
			if (documentId !== contentView.documentId){
				return
			}
			contentView.startLoading(documentId)
		}

		function onDocumentClosed(documentId){
			if (documentId !== contentView.documentId){
				return
			}
			contentView._destroyHosted()
			contentView.stopLoading(documentId)
		}

		function onCloseDocumentFailed(documentId, message){
			if (documentId !== contentView.documentId){
				return
			}
			contentView.stopLoading(documentId)
		}

		function onStartSaveDocument(documentId){
			if (documentId !== contentView.documentId){
				return
			}
			contentView.startLoading(documentId)
		}

		function onDocumentSaved(documentId){
			if (documentId !== contentView.documentId){
				return
			}
			contentView.stopLoading(documentId)
		}

		function onSaveDocumentFailed(documentId, message){
			if (documentId !== contentView.documentId){
				return
			}
			contentView.stopLoading(documentId)
			
			if(message !==""){
				PopupManager.addErrorMessage(message, true)
			}
		}

		function onStartUndo(documentId, steps){
			if (documentId !== contentView.documentId){
				return
			}
			contentView.startLoading(documentId)
		}

		function onUndoFailed(documentId, message){
			if (documentId !== contentView.documentId){
				return
			}
			contentView.stopLoading(documentId)
		}

		function onStartRedo(documentId, steps){
			if (documentId !== contentView.documentId){
				return
			}
			contentView.startLoading(documentId)
		}

		function onRedoFailed(documentId, message){
			if (documentId !== contentView.documentId){
				return
			}
			contentView.stopLoading(documentId)
		}
	}

	Rectangle {
		anchors.fill: parent
		color: Style.baseColor
	}

	Item {
		id: viewContainer
		anchors.fill: parent
	}

	Loading {
		id: loading
		z: parent.z + 1
		anchors.fill: parent
		visible: false
		opacity: contentView.showStandardLoading ? 1 : 0
		background.opacity: 0
	}

	onStartLoading: {
		if (documentId === contentView.documentId){
			loading.start()
		}
	}

	onStopLoading: {
		if (documentId === contentView.documentId){
			loading.stop()
		}
	}

	Component.onDestruction: {
		_destroyHosted()
	}
}
