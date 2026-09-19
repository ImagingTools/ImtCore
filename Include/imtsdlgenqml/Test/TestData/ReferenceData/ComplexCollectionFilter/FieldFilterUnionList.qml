import QtQuick
import imtcontrols 1.0

BaseModel {
	id: fieldFilterUnionList
	readonly property string __elementTypename: 'FieldFilterUnion'
	readonly property var allowedTypeNames: ['FieldFilter', 'ArrayFieldFilter']

	function isAllowedType(typename){
		return allowedTypeNames.indexOf(typename) !== -1
	}

	function createFieldFilter(){
		return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/FieldFilter.qml').createObject()
	}

	function addFieldFilter(element){
		if (!element){
			element = createFieldFilter()
		}
		appendElement(element)
		return element
	}

	function createArrayFieldFilter(){
		return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/ArrayFieldFilter.qml').createObject()
	}

	function addArrayFieldFilter(element){
		if (!element){
			element = createArrayFieldFilter()
		}
		appendElement(element)
		return element
	}

	function create(typename){
		switch (typename){
			case 'FieldFilter': return createFieldFilter()
			case 'ArrayFieldFilter': return createArrayFieldFilter()
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

	function addElement(element){
		if (element && isAllowedType(element.__typename)){
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

