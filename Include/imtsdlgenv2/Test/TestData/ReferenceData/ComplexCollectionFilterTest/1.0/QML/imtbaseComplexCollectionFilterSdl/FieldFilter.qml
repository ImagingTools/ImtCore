import QtQuick
import imtcontrols 1.0

BaseClass {
	id: fieldFilter
	readonly property string __typename: 'FieldFilter'
	property var m_fieldId
	property var m_filterValue
	property var m_filterValueType
	property var m_filterOperations

	function hasFieldId(){
		return (m_fieldId !== undefined && m_fieldId !== null)
	}

	function hasFilterValue(){
		return (m_filterValue !== undefined && m_filterValue !== null)
	}

	function hasFilterValueType(){
		return (m_filterValueType !== undefined && m_filterValueType !== null)
	}

	function hasFilterOperations(){
		return (m_filterOperations !== undefined && m_filterOperations !== null)
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_fieldId': return 'fieldId'
			case 'm_filterValue': return 'filterValue'
			case 'm_filterValueType': return 'filterValueType'
			case 'm_filterOperations': return 'filterOperations'
		}
	}

	function createElement(propertyId, typename){
		switch (propertyId){
		}
	}

	function createComponent(propertyId, typename){
		switch (propertyId){
		}
	}

	function createMe(){
		return Qt.createComponent('FieldFilter.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_fieldId': return 'string'
			case 'm_filterValue': return 'string'
			case 'm_filterValueType': return 'string'
			case 'm_filterOperations': return 'string'
		}
	}
}

