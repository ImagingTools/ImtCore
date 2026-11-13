import QtQuick
import imtcontrols 1.0

BaseClass {
	id: tableViewCondition
	readonly property string __typename: 'TableViewCondition'
	property string m_id: ""
	property BaseModel m_rows: BaseModel { owner: tableViewCondition }

	Component.onCompleted: {
		this._internal.removed = ["m_id","m_rows"]
	}

	function isArrayRequired(propertyId){
		switch (propertyId){
			case 'm_rows': return true
		}
		return false
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_id': return 'id'
			case 'm_rows': return 'rows'
			case '__typename': return '__typename'
		}
	}

	function createElement(propertyId, typename){
		switch (propertyId){
			case 'm_rows': return Qt.createComponent('TableViewRow.qml')
		}
	}

	function createComponent(propertyId, typename){
		let retVal;
		switch (propertyId){
			case 'm_rows':{
				retVal = Qt.createComponent('BaseModel.qml')
				retVal.owner = tableViewCondition
				return retVal
			}
		}
	}

	function createMe(){
		return Qt.createComponent('TableViewCondition.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_id': return 'string'
			case 'm_rows': return 'BaseModel'
		}
	}
}
