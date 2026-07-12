import QtQuick 2.12

// Representation controller for documents backed by any in-process backend
// exposing getLocalModel(documentId) — LocalDocumentServiceBackend, whether
// registered as pure in-memory or as a GqlRequestDocumentDataController-backed
// type. Because the backend and the view live in the same process,
// `representationModel` IS the document's canonical data object (same
// reference, not a copy pulled over the network), so there is nothing to
// actually transfer: both sync functions only need to fire the bookkeeping
// signals DocumentViewBinder listens for.
//
// Declare one Component per document type/view, binding `backend` to the
// LocalDocumentServiceBackend instance in scope, e.g.:
//
//   Component {
//       InProcessDocumentRepresentationController {
//           backend: myLocalBackend
//       }
//   }
DocumentRepresentationController {
	id: root

	property var backend: null

	representationModel: (backend && documentId) ? backend.getLocalModel(documentId) : null

	function updateRepresentationFromDocument(){
		startUpdateRepresentation(documentId, representationModel)
		representationUpdated(documentId, representationModel)
	}

	function updateDocumentFromRepresentation(){
		startUpdateDocument(documentId)
		documentUpdated(documentId)
	}
}
