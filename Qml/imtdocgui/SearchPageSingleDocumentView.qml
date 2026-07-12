import QtQuick 2.12
import imtdocgui 1.0

// Referenced by Partitura configuration (qrc:/qml/imtdocgui/SearchPageSingleDocumentView.qml).
// Historically this only forced the breadcrumb title hidden for the initial
// (collection) entry; DocumentTabsWorkspace's singleDocumentMode already
// hides its header whenever no document is drilled into, so no extra
// wiring is needed.
SingleDocumentWorkspacePageView {
	id: root
}
