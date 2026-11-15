import QtQuick
import imtcontrols 1.0

BaseClass {
	id: spectralColorMeasurementSpecification
	readonly property string __typename: 'SpectralColorMeasurementSpecification'
	property string m_aperture: ""
	property string m_backing: ""
	property string m_calibrationStandard: ""
	property DeviceInfo m_deviceInfo: null
	property string m_measurementType: ""
	property SpectrumInfo m_spectrumInfo: null
	property string m_spectrumType: ""

	Component.onCompleted: {
		this._internal.removed = ["m_aperture","m_backing","m_calibrationStandard","m_deviceInfo","m_measurementType","m_spectrumInfo","m_spectrumType"]
	}

	function isArrayRequired(propertyId){
		switch (propertyId){
		}
		return false
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_aperture': return 'aperture'
			case 'm_backing': return 'backing'
			case 'm_calibrationStandard': return 'calibrationStandard'
			case 'm_deviceInfo': return 'deviceInfo'
			case 'm_measurementType': return 'measurementType'
			case 'm_spectrumInfo': return 'spectrumInfo'
			case 'm_spectrumType': return 'spectrumType'
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
			case 'm_deviceInfo': return Qt.createComponent('DeviceInfo.qml')
			case 'm_spectrumInfo': return Qt.createComponent('SpectrumInfo.qml')
		}
	}

	function createMe(){
		return Qt.createComponent('SpectralColorMeasurementSpecification.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_aperture': return 'string'
			case 'm_backing': return 'string'
			case 'm_calibrationStandard': return 'string'
			case 'm_deviceInfo': return 'DeviceInfo'
			case 'm_measurementType': return 'string'
			case 'm_spectrumInfo': return 'SpectrumInfo'
			case 'm_spectrumType': return 'string'
		}
	}
}
