import QtQuick 2.12

QtObject {
	id: root

	property string documentId
	property string documentName
	property string documentTypeId
	property bool isDirty: false
	property bool isNew: true

	property var documentManager: null
	property var decorator: null
	property var collectionView: null
	// viewTypeId -> view mapping
	property var siblingViews: ({})
}
