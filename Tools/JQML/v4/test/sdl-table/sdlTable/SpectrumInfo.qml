import QtQuick
import imtcontrols 1.0

BaseClass {
	id: spectrumInfo
	readonly property string __typename: 'SpectrumInfo'
	property int m_end: 0
	property int m_start: 0
	property int m_step: 0

	Component.onCompleted: {
		this._internal.removed = ["m_end","m_start","m_step"]
	}

	function isArrayRequired(propertyId){
		switch (propertyId){
		}
		return false
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_end': return 'end'
			case 'm_start': return 'start'
			case 'm_step': return 'step'
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
		return Qt.createComponent('SpectrumInfo.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_end': return 'int'
			case 'm_start': return 'int'
			case 'm_step': return 'int'
		}
	}
}
