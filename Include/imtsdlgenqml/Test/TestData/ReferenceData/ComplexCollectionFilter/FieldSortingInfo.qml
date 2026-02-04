import QtQuick
import imtcontrols 1.0

BaseClass {
	id: fieldSortingInfo
	readonly property string __typename: 'FieldSortingInfo'
	property string m_fieldId: ''
	property string m_sortingOrder: ''

	Component.onCompleted: {
		this._internal.removed = ["m_fieldId","m_sortingOrder"]
	}

	function hasFieldId(){
		return (m_fieldId !== undefined && m_fieldId !== null)
	}

	function hasSortingOrder(){
		return (m_sortingOrder !== undefined && m_sortingOrder !== null)
	}

	function removeFieldId(){
		removeKey('m_fieldId')
	}

	function removeSortingOrder(){
		removeKey('m_sortingOrder')
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_fieldId': return 'fieldId'
			case 'm_sortingOrder': return 'sortingOrder'
			case '__typename': return '__typename'
		}
	}

	function createElement(propertyId, typename){
		switch (propertyId){
		}
	}

	function createComponent(propertyId, typename){
		let retVal;
		switch (propertyId){
		}
	}

	function createMe(){
		return Qt.createComponent('FieldSortingInfo.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_fieldId': return 'string'
			case 'm_sortingOrder': return 'string'
		}
	}
}

