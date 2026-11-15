import QtQuick
import imtcontrols 1.0

BaseClass {
	id: measurementValue
	readonly property string __typename: 'MeasurementValue'
	property var m_index: []
	property var m_value: []

	Component.onCompleted: {
		this._internal.removed = ["m_index","m_value"]
	}

	function isArrayRequired(propertyId){
		switch (propertyId){
		}
		return false
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_index': return 'index'
			case 'm_value': return 'value'
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
		return Qt.createComponent('MeasurementValue.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_index': return 'var'
			case 'm_value': return 'var'
		}
	}
}
