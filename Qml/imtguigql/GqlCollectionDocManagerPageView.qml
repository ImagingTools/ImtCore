import QtQuick 2.12
import imtdocgui 1.0
import imtguigql 1.0

DocumentServicePageView {
	id: root

	backendComp: Component {
		GqlDocumentServiceBackend {
			collectionId: root.pageId
		}
	}

	Component.onCompleted: {
		console.log("GqlCollectionDocManagerPageView.qml onCompleted", root.pageId)
	}

	function setDocumentName(documentId, name){}
}
