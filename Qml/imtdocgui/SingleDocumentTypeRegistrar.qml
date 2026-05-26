import QtQuick 2.12

// Declarative helper that registers one or more document view-types on a
// DocumentServiceBase without pulling in DocCollectionViewDelegate (which is
// tied to a CollectionView).
//
// Intended for single-document workspaces where there is no CollectionView to
// host the registration. Usage:
//
//   SingleDocumentTypeRegistrar {
//       documentManager: myService
//       views: [
//           { typeId: "TenantInfo", viewTypeId: "Editor",
//             editorComp: tenantEditorComp,
//             controllerComp: tenantRepresentationControllerComp }
//       ]
//   }
QtObject {
	id: root

	// Target service to register view-types into.
	property var documentManager: null

	// Array of registration descriptors. Each entry must provide:
	//   - typeId         : document type id (string)
	//   - viewTypeId     : view type id (string)
	//   - editorComp     : Component for the editor view (DocumentViewBase derivative)
	//   - controllerComp : Component for the representation controller
	property var views: []

	property bool _registered: false

	Component.onCompleted: {
		registerAll()
	}

	onDocumentManagerChanged: {
		_registered = false
		registerAll()
	}

	onViewsChanged: {
		_registered = false
		registerAll()
	}

	// Registers any not-yet-registered view descriptors with documentManager.
	// Safe to call multiple times.
	function registerAll(){
		if (!documentManager){
			return
		}

		if (!views || views.length === 0){
			return
		}

		for (let i = 0; i < views.length; ++i){
			let v = views[i]
			if (!v || !v.typeId || !v.viewTypeId || !v.editorComp || !v.controllerComp){
				console.error("SingleDocumentTypeRegistrar: invalid view descriptor at index " + i)
				continue
			}

			documentManager.registerDocumentViewData(
						v.typeId, v.viewTypeId, v.editorComp, v.controllerComp)
		}

		_registered = true
	}
}
