import QtQuick
import imtcontrols 1.0

BaseClass {
	id: rgbColorHex
	readonly property string __typename: 'RgbColorHex'
	property string m_color: ""

	Component.onCompleted: {
		this._internal.removed = ["m_color"]
	}

	function isArrayRequired(propertyId){
		switch (propertyId){
		}
		return false
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_color': return 'Color'
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
		return Qt.createComponent('RgbColorHex.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_color': return 'string'
		}
	}
}
