import QtQuick
import imtcontrols 1.0

BaseClass {
	id: coords
	readonly property string __typename: 'Coords'
	property var m_x
	property var m_y

	function hasX() {
		return (m_x !== undefined && m_x !== null)
	}

	function hasY() {
		return (m_y !== undefined && m_y !== null)
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_x': return 'X'
			case 'm_y': return 'Y'
		}
	}

	function createComponent(propertyId, typename){
		switch (propertyId){
			case 'm_filterInfo': return Qt.createComponent('qrc:/qml/imtbaseComplexCollectionFilterSdl/FieldSortingInfo.qml')
		}
	}

	function createMe(){
		return Qt.createComponent('Coords.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_x': return 'double'
			case 'm_y': return 'double'
		}
	}
}

