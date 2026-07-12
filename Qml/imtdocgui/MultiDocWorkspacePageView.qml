import QtQuick 2.12
import imtdocgui 1.0

// Generic tab-strip document page, referenced by Partitura configuration
// (qrc:/qml/imtdocgui/MultiDocWorkspacePageView.qml) — keep this file at
// this path/name even though its internals are new-architecture. Hosts a
// client-managed DocumentService (LocalDocumentServiceBackend): the actual
// document types are registered later, from wherever embeds a collection
// view that resolves this page via MainDocumentService.getDocumentService(pageId).
DocumentServicePageView {
	id: root

	backendComp: Component {
		LocalDocumentServiceBackend {}
	}
}
