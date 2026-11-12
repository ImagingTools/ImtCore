import QtQuick
import imtcontrols 1.0

BaseClass {
	id: lab
	readonly property string __typename: 'Lab'
	property double m_l: 0
	property double m_a: 0
	property double m_b: 0

	Component.onCompleted: {
		this._internal.removed = ["m_l","m_a","m_b"]
	}

	function isArrayRequired(propertyId){
		switch (propertyId){
		}
		return false
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_l': return 'L'
			case 'm_a': return 'a'
			case 'm_b': return 'b'
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
		return Qt.createComponent('Lab.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_l': return 'double'
			case 'm_a': return 'double'
			case 'm_b': return 'double'
		}
	}
}
