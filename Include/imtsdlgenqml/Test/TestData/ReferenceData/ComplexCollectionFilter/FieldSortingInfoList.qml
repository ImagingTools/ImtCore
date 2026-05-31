import QtQuick
import imtcontrols 1.0

BaseModel {
	id: fieldSortingInfoList
	readonly property string __elementTypename: 'FieldSortingInfo'
	readonly property var allowedTypeNames: ['FieldSortingInfo']

	function isAllowedType(typename){
		return allowedTypeNames.indexOf(typename) !== -1
	}

	function createFieldSortingInfo(){
		return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/FieldSortingInfo.qml').createObject()
	}

	function addFieldSortingInfo(){
		let element = createFieldSortingInfo()
		appendElement(element)
		return element
	}

	function create(typename){
		switch (typename){
			case 'FieldSortingInfo': return createFieldSortingInfo()
		}
		return null
	}

	function add(typename){
		let element = create(typename)
		if (element){
			appendElement(element)
		}
		return element
	}

	function findByType(typename){
		let retVal = []
		for (let i = 0; i < count; ++i){
			let item = get(i).item
			if (item && item.__typename === typename){
				retVal.push(item)
			}
		}
		return retVal
	}

	function countOfType(typename){
		return findByType(typename).length
	}
}

