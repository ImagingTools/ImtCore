import QtQuick
import imtcontrols 1.0

BaseClass {
	id: measurementValueView
	readonly property string __typename: 'MeasurementValueView'
	property double m_chroma: 0
	property double m_hue: 0
	property Lab m_lab: null
	property RgbColorHex m_preview: null

	Component.onCompleted: {
		this._internal.removed = ["m_chroma","m_hue","m_lab","m_preview"]
	}

	function isArrayRequired(propertyId){
		switch (propertyId){
		}
		return false
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_chroma': return 'chroma'
			case 'm_hue': return 'hue'
			case 'm_lab': return 'lab'
			case 'm_preview': return 'preview'
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
			case 'm_lab': return Qt.createComponent('Lab.qml')
			case 'm_preview': return Qt.createComponent('RgbColorHex.qml')
		}
	}

	function createMe(){
		return Qt.createComponent('MeasurementValueView.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_chroma': return 'double'
			case 'm_hue': return 'double'
			case 'm_lab': return 'Lab'
			case 'm_preview': return 'RgbColorHex'
		}
	}
}
