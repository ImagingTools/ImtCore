import QtQuick
import imtcontrols 1.0

BaseClass {
	id: condition
	readonly property string __typename: 'Condition'
	property string m_conditionId: ""
	property BaseModel m_measurementList: BaseModel { owner: condition }

	Component.onCompleted: {
		this._internal.removed = ["m_conditionId","m_measurementList"]
	}

	function isArrayRequired(propertyId){
		switch (propertyId){
			case 'm_measurementList': return true
		}
		return false
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_conditionId': return 'conditionId'
			case 'm_measurementList': return 'measurementList'
			case '__typename': return '__typename'
		}
	}

	function createElement(propertyId, typename){
		switch (propertyId){
			case 'm_measurementList': return Qt.createComponent('MeasurementItem.qml')
		}
	}

	function createComponent(propertyId, typename){
		let retVal;
		switch (propertyId){
			case 'm_measurementList':{
				retVal = Qt.createComponent('BaseModel.qml')
				retVal.owner = condition
				return retVal
			}
		}
	}

	function createMe(){
		return Qt.createComponent('Condition.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_conditionId': return 'string'
			case 'm_measurementList': return 'BaseModel'
		}
	}
}
