import QtQuick
import imtcontrols 1.0

BaseClass {
	id: link
	readonly property string __typename: 'Link'
	property var m_link: null

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_link': return 'link'
		}
	}

	function createComponent(propertyId, typename){
		switch (propertyId){

		}
	}

	function createMe(){
		return Qt.createComponent('Link.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_link': return 'string'
		}
	}
}

