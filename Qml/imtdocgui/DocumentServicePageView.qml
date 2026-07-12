import QtQuick 2.12

// Adds a DocumentTabsWorkspace (tab strip, or - with singleDocumentMode - a
// breadcrumb-style single-document view) on top of DocumentManagerPageBase,
// forwarding Partitura's startItemSourceComp into its embedded
// CollectionView slot. Concrete page files only need to set backendComp
// and, optionally, singleDocumentMode.
DocumentManagerPageBase {
	id: root

	// When true, the embedded workspace shows one document at a time
	// (breadcrumb-style header) instead of a tab strip, closing every other
	// open document whenever a new one is opened.
	property bool singleDocumentMode: false

	property alias documentManagerView: documentManagerView

	onStartItemSourceCompChanged: {
		if (startItemSourceComp){
			documentManagerView.setCollectionViewComp(pageName, startItemSourceComp)
		}
	}

	DocumentTabsWorkspace {
		id: documentManagerView
		anchors.fill: root
		documentManager: root.documentManager
		singleDocumentMode: root.singleDocumentMode
	}
}
