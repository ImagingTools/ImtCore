import QtQuick
import imtcontrols 1.0

BaseClass {
	id: printerSpecificationBase
	readonly property string __typename: 'PrinterSpecificationBase'
	property var m_name

	function hasName(){
		return (m_name !== undefined && m_name !== null)
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_name': return 'name'
		}
	}

	function createComponent(propertyId, typename){
		switch (propertyId){
		}
	}

	function createMe(){
		return Qt.createComponent('PrinterSpecificationBase.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_name': return 'string'
		}
	}
}

