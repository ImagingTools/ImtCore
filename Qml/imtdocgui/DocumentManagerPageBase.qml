import QtQuick 2.12
import imtgui 1.0

// Minimal shared plumbing every Partitura document-hosting page needs,
// regardless of whether it shows a browsable collection or drills straight
// into one known object: register the DocumentService under this page's
// pageId, and default-create the start item once the page becomes visible.
// Concrete pages differ only in which backend delegate they inject
// (backendComp) and what they embed alongside documentManager - that part
// lives in whatever extends this (e.g. DocumentServicePageView, which adds
// a collection-browsing workspace on top).
PageContainer {
	id: root

	// Component producing the backend delegate (GqlDocumentServiceBackend,
	// LocalDocumentServiceBackend, ...) instantiated once into
	// documentManager.backend. Concrete page files supply this so they can
	// bind delegate-specific properties (e.g. collectionId: root.pageId)
	// inside their own Component body.
	property Component backendComp

	property DocumentService documentManager: DocumentService {
		backend: root.backendComp ? root.backendComp.createObject(root) : null
	}

	onPageIdChanged: {
		if (pageId !== ""){
			MainDocumentService.registerDocumentService(pageId, documentManager)
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

	function setDocumentName(documentId, name){
		documentManager.setDocumentName(documentId, name)
	}
}
