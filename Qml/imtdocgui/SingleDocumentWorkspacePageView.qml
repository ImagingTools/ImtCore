import QtQuick 2.12
import imtdocgui 1.0

// Generic breadcrumb-style single-document page, referenced by Partitura
// configuration (qrc:/qml/imtdocgui/SingleDocumentWorkspacePageView.qml) —
// keep this file at this path/name even though its internals are
// new-architecture. Hosts a client-managed DocumentService
// (LocalDocumentServiceBackend); document types are registered later by
// whatever collection view embeds this page's registered documentManager.
DocumentServicePageView {
	id: root

	singleDocumentMode: true

	backendComp: Component {
		LocalDocumentServiceBackend {}
	}
}
