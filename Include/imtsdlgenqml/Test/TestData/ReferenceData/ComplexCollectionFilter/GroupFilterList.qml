import QtQuick
import imtcontrols 1.0

BaseModel {
	id: groupFilterList
	readonly property string __elementTypename: 'GroupFilter'
	readonly property var allowedTypeNames: ['GroupFilter']

	function isAllowedType(typename){
		return allowedTypeNames.indexOf(typename) !== -1
	}

	function createGroupFilter(){
		return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/GroupFilter.qml').createObject()
	}

	function addGroupFilter(element){
		if (!element){
			element = createGroupFilter()
		}
		appendElement(element)
		return element
	}

	function create(typename){
		switch (typename){
			case 'GroupFilter': return createGroupFilter()
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

