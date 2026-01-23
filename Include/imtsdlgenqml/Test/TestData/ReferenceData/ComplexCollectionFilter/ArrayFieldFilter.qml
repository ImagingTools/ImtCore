import QtQuick
import imtcontrols 1.0

BaseClass {
	id: arrayFieldFilter
	readonly property string __typename: 'ArrayFieldFilter'
	property string m_fieldId: ''
	property var m_filterValues: []
	property string m_filterValueType: ''
	property var m_filterOperations: []

	Component.onCompleted: {
		this._internal.removed = ["m_fieldId","m_filterValues","m_filterValueType","m_filterOperations"]
	}

	function hasFieldId(){
		return (m_fieldId !== undefined && m_fieldId !== null)
	}

	function hasFilterValues(){
		return (m_filterValues !== undefined && m_filterValues !== null)
	}

	function hasFilterValueType(){
		return (m_filterValueType !== undefined && m_filterValueType !== null)
	}

	function hasFilterOperations(){
		return (m_filterOperations !== undefined && m_filterOperations !== null)
	}

	function emplaceFilterValues(typename){
		m_filterValues = []
		this._internal.removeAt('m_filterValues')
	}

	function removeFieldId(){
		removeKey('m_fieldId')
	}

	function removeFilterValues(){
		removeKey('m_filterValues')
	}

	function removeFilterValueType(){
		removeKey('m_filterValueType')
	}

	function removeFilterOperations(){
		removeKey('m_filterOperations')
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_fieldId': return 'fieldId'
			case 'm_filterValues': return 'filterValues'
			case 'm_filterValueType': return 'filterValueType'
			case 'm_filterOperations': return 'filterOperations'
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
		return Qt.createComponent('ArrayFieldFilter.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_fieldId': return 'string'
			case 'm_filterValues': return 'array'
			case 'm_filterValueType': return 'string'
			case 'm_filterOperations': return 'BaseModel'
		}
	}
}

