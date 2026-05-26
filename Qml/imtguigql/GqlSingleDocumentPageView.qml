import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtgui 1.0
import imtdocgui 1.0

// Page wrapper that hosts a single document inside a workspace driven by
// the regular GqlBasedCollectionDocumentService (and therefore the
// server-side CollectionDocumentManager). No CollectionView is shown; the
// page is intended to navigate directly to a known object inside the
// collection.
//
// Routing: parameters can be supplied either as page-properties on the
// PageContainer (collectionId / objectId / objectTypeId / createNew) or
// through a NavigableItem path "<collectionId>/<typeId>/<objectId>" — the
// page wires both up.
PageContainer {
	id: root

	// Identifier of the collection on the server. Defaults to pageId so
	// that the page can be plugged into routing in the same way as
	// GqlCollectionDocManagerPageView.
	property string collectionId: root.pageId

	// Object id within the collection to be opened on activation.
	property string objectId: ""

	// Document type id used to look up the editor/representation
	// controller for the rendered document.
	property string objectTypeId: ""

	// When true and objectId is empty, the page creates a new document
	// instead of opening an existing one.
	property bool createNew: false

	// Shared service instance driving the workspace. Reuses
	// GqlBasedCollectionDocumentService so that this page benefits from
	// the existing server pipeline (subscriptions, open/save/close,
	// undo/redo) without duplicating any business logic.
	property DocumentServiceBase documentManager: GqlBasedCollectionDocumentService {
		collectionId: root.collectionId
	}

	Component.onCompleted: {
		console.log("GqlSingleDocumentPageView.qml onCompleted", root.pageId,
					"collectionId=", root.collectionId,
					"objectTypeId=", root.objectTypeId,
					"objectId=", root.objectId)
	}

	onPageIdChanged: {
		if (pageId !== ""){
			MainDocumentService.registerDocumentService(pageId, documentManager)
		}
	}

	onStartItemSourceCompChanged: {
		if (startItemSourceComp){
			// Backwards-compat hook (matches the convention of other gql
			// page wrappers). For single-document pages there is no
			// CollectionView to inject; the startItem (if any) is ignored.
		}
	}

	onVisibleChanged: {
		checkCreation()
	}

	onStartItemSourceChanged: {
		checkCreation()
	}

	function checkCreation(){
		if (!startItemIsCreated()){
			createStartItemComp()
		}
	}

	function setDocumentName(documentId, name){}

	// Programmatic open helper: callers can switch the rendered document
	// without rebuilding the page.
	function openObject(typeId, id){
		root.objectTypeId = typeId
		root.objectId = id
		root.createNew = false
	}

	// Programmatic create helper.
	function createObject(typeId){
		root.objectTypeId = typeId
		root.objectId = ""
		root.createNew = true
	}

	NavigableItem {
		id: navigableItem
		parentSegment: root.collectionId
		onActivated: {
			if (!root.documentManager){
				return
			}
			if (restPath.length >= 1){
				root.objectTypeId = matchedPath
				root.objectId = restPath[0]
				root.createNew = false
			}
			else if (params && params.createNew === true){
				root.objectTypeId = matchedPath
				root.objectId = ""
				root.createNew = true
			}
		}
	}

	Connections {
		target: root.documentManager
		function onDocumentViewRegistered(documentTypeId, viewTypeId){
			navigableItem.paths = root.documentManager.getSupportedDocumentTypeIds()
		}
	}

	SingleDocumentWorkspaceGqlView {
		id: workspaceView_
		anchors.fill: parent
		documentManager: root.documentManager
		objectId: root.objectId
		objectTypeId: root.objectTypeId
		createNew: root.createNew
	}
}
