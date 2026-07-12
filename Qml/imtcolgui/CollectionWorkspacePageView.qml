import QtQuick 2.12
import imtdocgui 1.0

// Referenced by Partitura configuration (qrc:/qml/imtcolgui/CollectionWorkspacePageView.qml)
// — keep this file at this path/name even though its internals are
// new-architecture. Byte-identical to imtdocgui/MultiDocWorkspacePageView.qml;
// kept as a separate file only because it lives in a different module and
// Partitura addresses it by that path.
//
// Note: the legacy version of this page additionally wired a
// GqlBasedObjectVisualStatusProvider into the tab strip (deriving tab
// names/icons from a live status subscription). DocumentTabsWorkspace does
// not have an equivalent hook yet; tab names fall back to the document's
// own name until that's added.
MultiDocWorkspacePageView {
	id: root
}
