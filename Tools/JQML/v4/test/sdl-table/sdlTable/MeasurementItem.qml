import QtQuick
import imtcontrols 1.0

BaseClass {
	id: measurementItem
	readonly property string __typename: 'MeasurementItem'
	property string m_measurementId: ""
	property MeasurementValue m_value: null
	property MeasurementValueView m_view: null

	Component.onCompleted: {
		this._internal.removed = ["m_measurementId","m_value","m_view"]
	}

	function isArrayRequired(propertyId){
		switch (propertyId){
		}
		return false
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_measurementId': return 'measurementId'
			case 'm_value': return 'value'
			case 'm_view': return 'view'
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
			case 'm_value': return Qt.createComponent('MeasurementValue.qml')
			case 'm_view': return Qt.createComponent('MeasurementValueView.qml')
		}
	}

	function createMe(){
		return Qt.createComponent('MeasurementItem.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_measurementId': return 'string'
			case 'm_value': return 'MeasurementValue'
			case 'm_view': return 'MeasurementValueView'
		}
	}
}
