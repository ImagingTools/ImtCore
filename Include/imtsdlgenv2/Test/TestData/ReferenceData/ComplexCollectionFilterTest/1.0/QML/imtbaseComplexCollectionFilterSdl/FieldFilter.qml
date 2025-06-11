import QtQuick
import imtcontrols 1.0

BaseClass {
	id: fieldFilter
	readonly property string __typename: 'FieldFilter'
	property var m_fieldId: null
	property var m_filterValue: null
	property var m_filterValueType: null
	property BaseModel m_filterOperations: []

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_fieldId': return 'fieldId'
			case 'm_filterValue': return 'filterValue'
			case 'm_filterValueType': return 'filterValueType'
			case 'm_filterOperations': return 'filterOperations'
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

