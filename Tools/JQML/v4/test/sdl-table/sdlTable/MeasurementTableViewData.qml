import QtQuick
import imtcontrols 1.0

BaseClass {
	id: measurementTableViewData
	readonly property string __typename: 'MeasurementTableViewData'
	property BaseModel m_colorantList: BaseModel { owner: measurementTableViewData }
	property BaseModel m_conditions: BaseModel { owner: measurementTableViewData }
	property BaseModel m_inkSetPreview: BaseModel { owner: measurementTableViewData }
	property string m_type: ""

	Component.onCompleted: {
		this._internal.removed = ["m_colorantList","m_conditions","m_inkSetPreview","m_type"]
	}

	function isArrayRequired(propertyId){
		switch (propertyId){
			case 'm_colorantList': return true
			case 'm_conditions': return true
			case 'm_inkSetPreview': return true
		}
		return false
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_colorantList': return 'colorantList'
			case 'm_conditions': return 'conditions'
			case 'm_inkSetPreview': return 'inkSetPreview'
			case 'm_type': return 'type'
			case '__typename': return '__typename'
		}
	}

	function createElement(propertyId, typename){
		switch (propertyId){
			case 'm_colorantList': return Qt.createComponent('ColorantItem.qml')
			case 'm_conditions': return Qt.createComponent('TableViewCondition.qml')
			case 'm_inkSetPreview': return Qt.createComponent('RgbColorHex.qml')
		}
	}

	function createComponent(propertyId, typename){
		let retVal;
		switch (propertyId){
			case 'm_colorantList':{
				retVal = Qt.createComponent('BaseModel.qml')
				retVal.owner = measurementTableViewData
				return retVal
			}
			case 'm_conditions':{
				retVal = Qt.createComponent('BaseModel.qml')
				retVal.owner = measurementTableViewData
				return retVal
			}
			case 'm_inkSetPreview':{
				retVal = Qt.createComponent('BaseModel.qml')
				retVal.owner = measurementTableViewData
				return retVal
			}
		}
	}

	function createMe(){
		return Qt.createComponent('MeasurementTableViewData.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_colorantList': return 'BaseModel'
			case 'm_conditions': return 'BaseModel'
			case 'm_inkSetPreview': return 'BaseModel'
			case 'm_type': return 'string'
		}
	}
}
