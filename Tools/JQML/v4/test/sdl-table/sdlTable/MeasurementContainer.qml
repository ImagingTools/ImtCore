import QtQuick
import imtcontrols 1.0

BaseClass {
	id: measurementContainer
	readonly property string __typename: 'MeasurementContainer'
	property SpectralColorMeasurementSpecification m_colorSpecification: null
	property BaseModel m_colorantList: BaseModel { owner: measurementContainer }
	property BaseModel m_conditionList: BaseModel { owner: measurementContainer }

	Component.onCompleted: {
		this._internal.removed = ["m_colorSpecification","m_colorantList","m_conditionList"]
	}

	function isArrayRequired(propertyId){
		switch (propertyId){
			case 'm_colorantList': return true
			case 'm_conditionList': return true
		}
		return false
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_colorSpecification': return 'colorSpecification'
			case 'm_colorantList': return 'colorantList'
			case 'm_conditionList': return 'conditionList'
			case '__typename': return '__typename'
		}
	}

	function createElement(propertyId, typename){
		switch (propertyId){
			case 'm_colorantList': return Qt.createComponent('ColorantItem.qml')
			case 'm_conditionList': return Qt.createComponent('Condition.qml')
		}
	}

	function createComponent(propertyId, typename){
		let retVal;
		switch (propertyId){
			case 'm_colorSpecification': return Qt.createComponent('SpectralColorMeasurementSpecification.qml')
			case 'm_colorantList':{
				retVal = Qt.createComponent('BaseModel.qml')
				retVal.owner = measurementContainer
				return retVal
			}
			case 'm_conditionList':{
				retVal = Qt.createComponent('BaseModel.qml')
				retVal.owner = measurementContainer
				return retVal
			}
		}
	}

	function createMe(){
		return Qt.createComponent('MeasurementContainer.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_colorSpecification': return 'SpectralColorMeasurementSpecification'
			case 'm_colorantList': return 'BaseModel'
			case 'm_conditionList': return 'BaseModel'
		}
	}
}
