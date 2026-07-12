import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

// In-process, single-user transport for DocumentService: there is no
// server-side multi-user document session, no undo transactions shared
// across users, and no subscription push — the document's lifecycle
// (open/create/save/close/undo/redo) is entirely managed on the client.
// This is the direct replacement for the old client-only DocumentService.
//
// Per document type, register EITHER:
//   - registerDocumentType(typeId, modelComp)
//     Pure in-memory storage: documents are created/loaded/saved against a
//     plain store kept by this backend, nothing ever leaves the process.
//     Good for demos, tests, or genuinely local-only data.
//   - registerDocumentDataController(typeId, dataControllerComp)
//     Real persistence, delegated to a data-controller instance (typically
//     GqlRequestDocumentDataController, see imtguigql) that this backend
//     drives through its createDocumentModel()/updateDocumentModel()/
//     insertDocument()/saveDocument() contract. The document's *name* is
//     then considered data-derived (hasNameProvider=true): the controller's
//     own `documentName` binding is authoritative, not the name argument
//     passed to save().
//
// Either way, undo/redo and the dirty flag are provided uniformly by a
// per-document UndoRedoManager snapshotting the data object — real,
// client-side undo/redo of in-progress edits, independent of how (or
// whether) the data eventually gets persisted.
//
// Because the backend and the view live in the same process, there is no
// separate "pull representation from the server" round trip: the model
// instance returned by getLocalModel() IS the document's canonical data.
// Pair this backend with InProcessDocumentRepresentationController, which
// reads that model by reference instead of copying it over the network.
QtObject {
	id: root

	// typeId -> Component (creates a fresh, empty data object for that type)
	property var __typeFactories: ({})

	// typeId -> Component (creates a data-controller instance for that type)
	property var __dataControllerFactories: ({})

	// documentId -> {typeId, objectId, name, model, undoManager, isNew, controller?}
	property var __documents: ({})

	// objectId -> {typeId, name, model}  (the "saved" store, pure in-memory types only)
	property var __store: ({})

	// Required by the DocumentService backend contract; this backend never
	// pushes unsolicited events (single-user, nothing to broadcast), but the
	// signals still need to exist so DocumentService can connect to them.
	signal documentEvent(string operation, string objectId, string documentId, string documentName, bool isDirty, bool isLoading)
	signal undoChanged(string documentId, int undoSteps, int redoSteps, bool isDirty)

	function registerDocumentType(typeId, modelComp){
		__typeFactories[typeId] = modelComp
	}

	function registerDocumentDataController(typeId, dataControllerComp){
		__dataControllerFactories[typeId] = dataControllerComp
	}

	// Returns the live data object backing `documentId`, or null. The
	// returned object is the same reference the backend holds — callers may
	// read and write it directly.
	function getLocalModel(documentId){
		let entry = __documents[documentId]
		return entry ? entry.model : null
	}

	// Dirty means "differs from the last-saved (or last-loaded) baseline",
	// mirroring the server-side IDocumentStateComparator semantics: computed
	// on demand from the undo manager's standard/baseline model rather than
	// tracked with a separate flag, so it self-corrects when the user undoes
	// back to a clean state.
	function __isDirty(entry){
		let baseline = entry.undoManager.getStandardModel()
		return !baseline || !entry.model.isEqualWithModel(baseline)
	}

	// ---- Backend contract ------------------------------------------------

	function getOpenedDocumentList(callback){
		// Nothing survives across sessions for a client-managed backend:
		// every createDocument/openDocument call starts a fresh entry.
		callback({ok: true, documents: []})
	}

	function createDocument(typeId, proposedObjectId, callback){
		let documentId = proposedObjectId || UuidGenerator.generateUUID()

		let controllerFactory = __dataControllerFactories[typeId]
		if (controllerFactory){
			let controller = controllerFactory.createObject(root, {documentId: documentId, typeId: typeId})
			controller.createDocumentModel()

			__documents[documentId] = {
				typeId: typeId,
				objectId: "",
				name: "",
				model: controller.documentModel,
				undoManager: __createUndoManager(controller.documentModel),
				isNew: true,
				controller: controller
			}

			callback({
				ok: true,
				documentId: documentId,
				objectId: "",
				typeId: typeId,
				name: "",
				hasNameProvider: true,
				isDirty: false,
				isLoading: false
			})
			return
		}

		let factory = __typeFactories[typeId]
		if (!factory){
			callback({ok: false, message: "Unknown local document type '" + typeId + "'"})
			return
		}

		let model = factory.createObject(root)

		__documents[documentId] = {
			typeId: typeId,
			objectId: "",
			name: "",
			model: model,
			undoManager: __createUndoManager(model),
			isNew: true
		}

		callback({
			ok: true,
			documentId: documentId,
			objectId: "",
			typeId: typeId,
			name: "",
			hasNameProvider: false,
			isDirty: false,
			isLoading: false
		})
	}

	function openDocument(typeId, objectId, callback){
		let controllerFactory = __dataControllerFactories[typeId]
		if (controllerFactory){
			// No separate document-session id for controller-backed types:
			// the document IS the object, addressed by its own id.
			let documentId = objectId
			let controller = controllerFactory.createObject(root, {documentId: documentId, typeId: typeId})

			let onModelChanged = function(){
				controller.modelChanged.disconnect(onModelChanged)
				controller.error.disconnect(onError)

				root.__documents[documentId] = {
					typeId: typeId,
					objectId: objectId,
					name: controller.documentName,
					model: controller.documentModel,
					undoManager: root.__createUndoManager(controller.documentModel),
					isNew: false,
					controller: controller
				}

				callback({
					ok: true,
					documentId: documentId,
					objectId: objectId,
					typeId: typeId,
					name: controller.documentName,
					hasNameProvider: true,
					isDirty: false,
					isLoading: false
				})
			}
			let onError = function(message, type){
				controller.modelChanged.disconnect(onModelChanged)
				controller.error.disconnect(onError)
				controller.destroy()
				callback({ok: false, message: message || "Failed to load document"})
			}

			controller.modelChanged.connect(onModelChanged)
			controller.error.connect(onError)
			controller.updateDocumentModel()
			return
		}

		let stored = __store[objectId]
		if (!stored){
			callback({ok: false, message: "Object '" + objectId + "' not found"})
			return
		}

		let factory = __typeFactories[stored.typeId]
		if (!factory){
			callback({ok: false, message: "Unknown local document type '" + stored.typeId + "'"})
			return
		}

		let model = factory.createObject(root)
		model.copyFrom(stored.model)

		let documentId = UuidGenerator.generateUUID()
		__documents[documentId] = {
			typeId: stored.typeId,
			objectId: objectId,
			name: stored.name,
			model: model,
			undoManager: __createUndoManager(model),
			isNew: false
		}

		callback({
			ok: true,
			documentId: documentId,
			objectId: objectId,
			typeId: stored.typeId,
			name: stored.name,
			hasNameProvider: false,
			isDirty: false,
			isLoading: false
		})
	}

	function saveDocument(documentId, documentName, callback){
		let entry = __documents[documentId]
		if (!entry){
			callback({ok: false, message: "Unknown document"})
			return
		}

		if (entry.controller){
			let onSaved = function(savedId, savedName){
				entry.controller.saved.disconnect(onSaved)
				entry.controller.error.disconnect(onError)

				entry.isNew = false
				if (savedId){
					entry.objectId = savedId
				}
				entry.name = entry.controller.getDocumentName()
				entry.undoManager.setStandardModel(entry.model)

				callback({ok: true, documentName: entry.name})
			}
			let onError = function(message, type){
				entry.controller.saved.disconnect(onSaved)
				entry.controller.error.disconnect(onError)
				callback({ok: false, message: message || "Failed to save document"})
			}

			entry.controller.saved.connect(onSaved)
			entry.controller.error.connect(onError)

			if (entry.isNew){
				entry.controller.insertDocument()
			}
			else{
				entry.controller.saveDocument()
			}
			return
		}

		let objectId = entry.objectId || UuidGenerator.generateUUID()
		entry.objectId = objectId
		entry.name = documentName
		entry.isNew = false
		entry.undoManager.setStandardModel(entry.model)

		let savedModel = __typeFactories[entry.typeId].createObject(root)
		savedModel.copyFrom(entry.model)
		__store[objectId] = {typeId: entry.typeId, name: documentName, model: savedModel}

		callback({ok: true, documentName: documentName})
	}

	function closeDocument(documentId, callback){
		let entry = __documents[documentId]
		if (entry){
			entry.undoManager.destroy()
			if (entry.controller){
				entry.controller.destroy()
			}
			else{
				entry.model.destroy()
			}
			delete __documents[documentId]
		}

		callback({ok: true})
	}

	function doUndo(documentId, steps, callback){
		let entry = __documents[documentId]
		if (!entry || entry.undoManager.getAvailableUndoSteps() < steps){
			callback({ok: false, message: "Undo failed"})
			return
		}

		for (let i = 0; i < steps; ++i){
			entry.undoManager.doUndo()
		}

		callback({ok: true})
	}

	function doRedo(documentId, steps, callback){
		let entry = __documents[documentId]
		if (!entry || entry.undoManager.getAvailableRedoSteps() < steps){
			callback({ok: false, message: "Redo failed"})
			return
		}

		for (let i = 0; i < steps; ++i){
			entry.undoManager.doRedo()
		}

		callback({ok: true})
	}

	function resetUndo(documentId, callback){
		let entry = __documents[documentId]
		if (!entry){
			callback({ok: false, message: "Unknown document"})
			return
		}

		entry.undoManager.resetUndo()
		callback({ok: true})
	}

	function getUndoInfo(documentId, callback){
		let entry = __documents[documentId]
		if (!entry){
			callback({ok: false, message: "Unknown document"})
			return
		}

		callback({
			ok: true,
			undoSteps: entry.undoManager.getAvailableUndoSteps(),
			redoSteps: entry.undoManager.getAvailableRedoSteps(),
			isDirty: root.__isDirty(entry)
		})
	}

	// ---- internals ---------------------------------------------------------

	property Component __undoManagerComp: Component {
		UndoRedoManager {}
	}

	function __createUndoManager(model){
		let undoManager = __undoManagerComp.createObject(root)
		undoManager.registerModel(model)
		return undoManager
	}
}
