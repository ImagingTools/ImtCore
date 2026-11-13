import QtQuick
import imtcontrols 1.0

BaseClass {
	id: measurementViewData
	readonly property string __typename: 'MeasurementViewData'
	property MeasurementContainer m_measurements: null
	property MeasurementTableViewData m_table: null

	Component.onCompleted: {
		this._internal.removed = ["m_measurements","m_table"]
	}

	function isArrayRequired(propertyId){
		switch (propertyId){
		}
		return false
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_measurements': return 'measurements'
			case 'm_table': return 'table'
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
			case 'm_measurements': return Qt.createComponent('MeasurementContainer.qml')
			case 'm_table': return Qt.createComponent('MeasurementTableViewData.qml')
		}
	}

	function createMe(){
		return Qt.createComponent('MeasurementViewData.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_measurements': return 'MeasurementContainer'
			case 'm_table': return 'MeasurementTableViewData'
		}
	}
}
