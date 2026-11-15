import QtQuick
import imtcontrols 1.0

BaseClass {
	id: colorantItem
	readonly property string __typename: 'ColorantItem'
	property ColorantProperties m_colorPreview: null
	property string m_colorantName: ""

	Component.onCompleted: {
		this._internal.removed = ["m_colorPreview","m_colorantName"]
	}

	function isArrayRequired(propertyId){
		switch (propertyId){
		}
		return false
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_colorPreview': return 'colorPreview'
			case 'm_colorantName': return 'colorantName'
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
			case 'm_colorPreview': return Qt.createComponent('ColorantProperties.qml')
		}
	}

	function createMe(){
		return Qt.createComponent('ColorantItem.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_colorPreview': return 'ColorantProperties'
			case 'm_colorantName': return 'string'
		}
	}
}
