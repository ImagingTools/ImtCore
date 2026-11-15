import QtQuick
import imtcontrols 1.0

BaseClass {
	id: deviceInfo
	readonly property string __typename: 'DeviceInfo'
	property string m_deviceClass: ""
	property string m_deviceModel: ""
	property string m_devicePolarization: ""
	property string m_filterType: ""
	property string m_illuminationType: ""
	property string m_manufacturer: ""
	property string m_serialNumber: ""

	Component.onCompleted: {
		this._internal.removed = ["m_deviceClass","m_deviceModel","m_devicePolarization","m_filterType","m_illuminationType","m_manufacturer","m_serialNumber"]
	}

	function isArrayRequired(propertyId){
		switch (propertyId){
		}
		return false
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_deviceClass': return 'deviceClass'
			case 'm_deviceModel': return 'deviceModel'
			case 'm_devicePolarization': return 'devicePolarization'
			case 'm_filterType': return 'filterType'
			case 'm_illuminationType': return 'illuminationType'
			case 'm_manufacturer': return 'manufacturer'
			case 'm_serialNumber': return 'serialNumber'
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
		return Qt.createComponent('DeviceInfo.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_deviceClass': return 'string'
			case 'm_deviceModel': return 'string'
			case 'm_devicePolarization': return 'string'
			case 'm_filterType': return 'string'
			case 'm_illuminationType': return 'string'
			case 'm_manufacturer': return 'string'
			case 'm_serialNumber': return 'string'
		}
	}
}
