import QtQuick
import imtcontrols 1.0

BaseClass {
	id: printerBase
	readonly property string __typename: 'PrinterBase'
	property var m_name
	property var m_specification

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_name': return 'name'
			case 'm_specification': return 'specification'
		}
	}

	function createComponent(propertyId, typename){
		switch (propertyId){
			case 'm_specification': {
				switch (typename){
					case 'PrinterSpecificationBase': return Qt.createComponent('qrc:/qml/modsdlUnionTestSdl/PrinterSpecificationBase.qml')
					case 'Link': return Qt.createComponent('qrc:/qml/modsdlUnionTestSdl/Link.qml')
				}
			}
		}
	}

	function createMe(){
		return Qt.createComponent('PrinterBase.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_name': return 'string'
			case 'm_specification': {
				if (!m_specification || !m_specification.__typename){
					return 'PrinterSpecification'
				}
				else {
					return m_specification.__typename
				}
			}
		}
	}
}

