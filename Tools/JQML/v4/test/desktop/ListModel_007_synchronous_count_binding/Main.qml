import QtQuick

Item {
	id: root
	property int documentCount: documents.count
	property var documentIndex: ({})
	signal documentAdded(string documentId)

	ListModel { id: documents }

	onDocumentCountChanged: {
		let result = ({})
		for (let i = 0; i < documents.count; i++){
			result[documents.get(i).id] = i
		}
		documentIndex = result
	}

	Connections {
		target: root
		function onDocumentAdded(documentId){
			let index = root.documentIndex[documentId]
			console.log("added", documentId, root.documentCount, index, documents.get(index).viewComp)
		}
	}

	Component.onCompleted: {
		documents.append({"id": "new", "viewComp": "newView"})
		documentAdded("new")
		documents.insert(0, {"id": "existing", "viewComp": "existingView"})
		documentAdded("existing")
		console.log("moved", documentIndex["new"])
		documents.remove(0)
		console.log("remaining", documentCount, documentIndex["new"])
		documents.clear()
		console.log("cleared", documentCount, Object.keys(documentIndex).length)
		Qt.quit()
	}
}
