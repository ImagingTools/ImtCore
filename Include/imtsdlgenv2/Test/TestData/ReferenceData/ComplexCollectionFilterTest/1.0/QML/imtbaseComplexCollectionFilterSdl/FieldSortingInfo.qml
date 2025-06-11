import QtQuick
import imtcontrols 1.0

BaseClass {
	id: fieldSortingInfo
	readonly property string __typename: 'FieldSortingInfo'
	property var m_fieldId: null
	property var m_sortingOrder: null

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_fieldId': return 'fieldId'
			case 'm_sortingOrder': return 'sortingOrder'
		}
	}

	function createComponent(propertyId, typename){
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

