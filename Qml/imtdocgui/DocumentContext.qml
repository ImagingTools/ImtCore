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
	property var siblingViews: []

	signal siblingViewDataBroadcast(var sourceView, string channel, var data)

	function getSiblingViews(excludeView){
		let result = []

		for (let i = 0; i < siblingViews.length; ++i){
			if (siblingViews[i] !== excludeView){
				result.push(siblingViews[i])
			}
		}

		return result
	}

	function broadcastToSiblings(sourceView, channel, data){
		siblingViewDataBroadcast(sourceView, channel, data)
	}
}
