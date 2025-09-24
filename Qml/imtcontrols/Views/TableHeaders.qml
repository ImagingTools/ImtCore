import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtbaseImtCollectionSdl 1.0

BaseModel {
	id: root
	
	Component {
		id: headerInfoComp
		HeaderInfo {
		}
	}

	function addHeader(id, name, filterable, sortable){
		let headerObj = headerInfoComp.createObject(root)
		headerObj.m_id = id
		headerObj.m_name = name

		if (filterable === undefined){
			filterable = true
		}
		headerObj.m_filterable = filterable

		if (sortable === undefined){
			sortable = true
		}
		headerObj.m_sortable = sortable

		addElement(headerObj)
	}

	function removeHeader(id){
		let index = getHeaderIndex(id)
		if (index < 0){
			return false
		}
		removeElement(index)
		return true
	}

	function getHeaderIndex(id){
		for (let i = 0; i < this.count; ++i){
			let headerId = this.get(i).item.m_id
			if (headerId === id){
				return i
			}
		}
		return -1
	}

	function setHeaderName(id, name){
		let index = getHeaderIndex(id)
		if (index < 0){
			return false
		}
		get(index).item.m_name = name

		return true
	}
}

