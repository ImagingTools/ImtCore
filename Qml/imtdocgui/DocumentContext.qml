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
	// viewTypeId -> view mapping
	property var siblingViews: ({})

	signal siblingViewDataBroadcast(var sourceView, string channel, var data)

	function getSiblingView(viewTypeId){
		if (viewTypeId in siblingViews){
			return siblingViews[viewTypeId]
		}

		return null
	}

	function getSiblingViews(excludeView){
		let result = {}
		let keys = Object.keys(siblingViews)

		for (let i = 0; i < keys.length; ++i){
			if (siblingViews[keys[i]] !== excludeView){
				result[keys[i]] = siblingViews[keys[i]]
			}
		}

		return result
	}

	function broadcastToSiblings(sourceView, channel, data){
		siblingViewDataBroadcast(sourceView, channel, data)
	}
}
